#include "../header_files/Coding.h"

Coding::Coding(const char *filename) {
    //Open the image and split it into pieces.

    // Open image
    this->pixbuf = gdk_pixbuf_new_from_file(filename, &this->error);
    /*if (!pixbuf) {
        g_print(stderr, "Error loading image: %s\n", error->message);
        g_error_free(error);
        return;
    }*/

    // Compare pixel colors and create automata.

    this->width = gdk_pixbuf_get_width(this->pixbuf);
    this->height = gdk_pixbuf_get_height(this->pixbuf);
    this->coding_image_size = this->width > this->height ? this->height : this->width;

    /*AverageCounter(pixbuf, &full_image);

    g_print("%d, %d, %d, %f, %f, %f\n", full_image.x, full_image.y, full_image.size, full_image.color[0], full_image.color[1], full_image.color[2]);*/

    // A full quadtree for a (2^level*2^level) image has ((4^level - 1) / 3) nodes.
    // The number of leaves in a full quadtree is (4^level).
    // We don't want to store the leaves.
    this->depth = static_cast <int> (log2(this->coding_image_size)); // maximum recursion level
    this->quadtree_size_1 = 0;

    // In this iteration the maximum value of i is depth-1
    // Save the size of depth-3, depth-2, depth-1 quadtrees
    for (int i = 0; i < this->depth; ++i) {
        this->quadtree_size_1 += pow(4, i);
        if(i == this->depth - 3) {
            this->quadtree_size_3 = this->quadtree_size_1;
        } else if(i == this->depth - 2) {
            this->quadtree_size_2 = this->quadtree_size_1;
        }
    }

    this->quadtree = new Quadrant*[this->quadtree_size_1];
    // Initial basis
    int initial_basis_size = 1;

    this->F = gsl_matrix_alloc(initial_basis_size, 1);

    this->A = gsl_matrix_alloc(initial_basis_size, initial_basis_size);
    this->B = gsl_matrix_alloc(initial_basis_size, initial_basis_size);
    this->C = gsl_matrix_alloc(initial_basis_size, initial_basis_size);
    this->D = gsl_matrix_alloc(initial_basis_size, initial_basis_size);

    gsl_matrix_set_zero(this->A);
    gsl_matrix_set_zero(this->B);
    gsl_matrix_set_zero(this->C);
    gsl_matrix_set_zero(this->D);
    gsl_matrix_set_zero(this->F);
}

Coding::~Coding() {
    // Free the allocated memory.
    g_object_unref(this->pixbuf);

    for (int i = 0; i < this->quadtree_size_1; ++i) {
        delete this->quadtree[i];
        this->quadtree[i] = nullptr;
    }

    delete[] this->quadtree;
    this->quadtree = nullptr;

    // Save states_count into a local variable, because if a state is deleted, states_count is decremented by 1.
    int states_number = State::states_count;

    for (int i = 0; i < states_number; ++i) {
        delete this->states[i];
        this->states[i] = nullptr;
    }

    delete[] this->states;
    this->states = nullptr;

    gsl_matrix_free(this->A);
    gsl_matrix_free(this->B);
    gsl_matrix_free(this->C);
    gsl_matrix_free(this->D);
    gsl_matrix_free(this->F);
}

void Coding::Start() {
    CreateQuadtree(this->depth, 0, 0, 0);
    ChildPointers();

    /*for (int i = 0; i < this->quadtree_size_1; i++) {
        g_print("%f ", this->quadtree[i]->brightness);
    }*/

    CreateWFA(0.1);

    SaveWFA("/home/jani/wfa.wfa");
}

void Coding::CreateWFA(double epsilon) {

    // First state represents the whole image.
    if(State::states_count == 0) {
        this->states = new State*[1];
        //Index is 0 by default
        this->states[0] = new State(this->quadtree[0]);
    }

    for (int i = 0; i < State::states_count; ++i) {

        State *scanned_state = this->states[i];

        ScanState(scanned_state->a, 'a', i);
        ScanState(scanned_state->b, 'b', i);
        ScanState(scanned_state->c, 'c', i);
        ScanState(scanned_state->d, 'd', i);
    }
}

void Coding::ScanState(Quadrant *quadrant, char quadrant_symbol, int state_index) {

    // state image vs quadrant

    State *state_image = this->states[state_index];

    gsl_matrix *Acpy, *Bcpy, *Ccpy, *Dcpy, *Fcpy;
    State **statescpy;

    // Compare scanned quadrant's children and appropriate grandchildren of a parent.
    // quadrant vs state_image

    double cost = CompareQuadrants(this->depth - state_index, quadrant, state_image);
    if( cost != -1 ) {

        // If costs are equal add new transition into the appropriate matrix.

        switch (quadrant_symbol) {
            case 'a':
                gsl_matrix_set(this->A, quadrant->parent->index, state_image->index, cost );
            break;
            case 'b':
                gsl_matrix_set(this->B, quadrant->parent->index, state_image->index, cost );
            break;
            case 'c':
                gsl_matrix_set(this->C, quadrant->parent->index, state_image->index, cost );
            break;
            case 'd':
                gsl_matrix_set(this->D, quadrant->parent->index, state_image->index, cost );
            break;
        }

    }else if(state_index < State::states_count - 1){

        // If not, check other states
        ScanState(quadrant, quadrant_symbol, state_index + 1);

    } else {
        // Create new state

        // Save matrices into initial matrices to copy data
        Acpy = gsl_matrix_alloc(State::states_count, State::states_count);
        Bcpy = gsl_matrix_alloc(State::states_count, State::states_count);
        Ccpy = gsl_matrix_alloc(State::states_count, State::states_count);
        Dcpy = gsl_matrix_alloc(State::states_count, State::states_count);
        Fcpy = gsl_matrix_alloc(State::states_count, 1);

        gsl_matrix_memcpy(Acpy, this->A);
        gsl_matrix_memcpy(Bcpy, this->B);
        gsl_matrix_memcpy(Ccpy, this->C);
        gsl_matrix_memcpy(Dcpy, this->D);
        gsl_matrix_memcpy(Fcpy, this->F);

        gsl_matrix_free(this->A);
        gsl_matrix_free(this->B);
        gsl_matrix_free(this->C);
        gsl_matrix_free(this->D);
        gsl_matrix_free(this->F);

        // Add new state and set the appropriate matrix index (line and col).

        // Copy states into a saved array.
        statescpy = new State*[State::states_count];
        for (int i = 0; i < State::states_count; ++i) {
            statescpy[i] = this->states[i];
        }

        // Do not free the elements of states[], because these are saved into statescpy[].
        // Just free the state[] itself and create a bigger one.
        delete[] this->states;
        this->states = nullptr;

        // If I create new state, states_count increments automatically.
        State *new_state = new State(quadrant);
        this->states = new State *[State::states_count];

        // Copy the saved states into states[]
        for (int i = 0; i < State::states_count - 1; ++i) {
            this->states[i] = statescpy[i];
        }

        // Do not free the elements of statescpy[], because these are copied into states[].
        // Just free the statescpy[] itself.
        delete[] statescpy;
        statescpy = nullptr;

        //Add new state to states
        states[State::states_count - 1] = new_state;

        //New state represents the quadrant, so they have same index.
        quadrant->index = new_state->index;

        // expand the matrices
        this->A = gsl_matrix_alloc(State::states_count, State::states_count);
        this->B = gsl_matrix_alloc(State::states_count, State::states_count);
        this->C = gsl_matrix_alloc(State::states_count, State::states_count);
        this->D = gsl_matrix_alloc(State::states_count, State::states_count);
        this->F = gsl_matrix_alloc(State::states_count, 1);

        gsl_matrix_set_zero(this->A);
        gsl_matrix_set_zero(this->B);
        gsl_matrix_set_zero(this->C);
        gsl_matrix_set_zero(this->D);
        gsl_matrix_set_zero(this->F);

        // Copy the contents of smaller matrices to bigger ones using initial matrices.
        for (int i = 0; i < State::states_count; ++i) {
            if( i < State::states_count - 1 ) {
                gsl_matrix_set(this->F, i, 0, gsl_matrix_get(Fcpy, i, 0));

                for (int j = 0; j < State::states_count - 1; ++j) {
                    gsl_matrix_set(this->A, i, j, gsl_matrix_get(Acpy, i, j));
                    gsl_matrix_set(this->B, i, j, gsl_matrix_get(Bcpy, i, j));
                    gsl_matrix_set(this->C, i, j, gsl_matrix_get(Ccpy, i, j));
                    gsl_matrix_set(this->D, i, j, gsl_matrix_get(Dcpy, i, j));
                }
            } else {
                // Add new quadrant to F.
                gsl_matrix_set(this->F, i, 0, quadrant->brightness);
            }
        }

        //Set transition
        switch (quadrant_symbol) {
            case 'a':
                gsl_matrix_set(this->A, quadrant->parent->index, new_state->index, 1 );
            break;
            case 'b':
                gsl_matrix_set(this->B, quadrant->parent->index, new_state->index, 1 );
            break;
            case 'c':
                gsl_matrix_set(this->C, quadrant->parent->index, new_state->index, 1 );
            break;
            case 'd':
                gsl_matrix_set(this->D, quadrant->parent->index, new_state->index, 1 );
            break;
        }

        gsl_matrix_free(Acpy);
        gsl_matrix_free(Bcpy);
        gsl_matrix_free(Ccpy);
        gsl_matrix_free(Dcpy);
        gsl_matrix_free(Fcpy);
    }
}

double Coding::CompareQuadrants(int level, Quadrant *q1, Quadrant *q2) {
    double cost_a, cost_b, cost_c, cost_d;
    if(level > 1) {

        cost_a = CompareQuadrants(level - 1, q1->a, q2->a);
        cost_b = CompareQuadrants(level - 1, q1->b, q2->b);
        cost_c = CompareQuadrants(level - 1, q1->c, q2->c);
        cost_d = CompareQuadrants(level - 1, q1->d, q2->d);

    } else {
        cost_a = q1->a->brightness / q2->a->brightness;
        cost_b = q1->b->brightness / q2->b->brightness;
        cost_c = q1->c->brightness / q2->c->brightness;
        cost_d = q1->d->brightness / q2->d->brightness;
    }

    if( abs(cost_a - cost_b) < this->EPS && abs(cost_a - cost_c) < this->EPS && abs(cost_a - cost_d) < this->EPS  ){
        return cost_a;
    }
    return -1;
}


double Coding::CreateQuadtree(int level, int index, int x, int y){
    // Count the average color of the image segment (quadrant) according to parameters.

    if(level > 0) {
        const int quadrant_size = 1 << level; // 2^level
        // The average colors of quadrants
        const double A = CreateQuadtree(level-1, 4*index + 1, x, y);
        const double B = CreateQuadtree(level-1, 4*index + 2, x + quadrant_size/2, y);
        const double C = CreateQuadtree(level-1, 4*index + 3, x, y + quadrant_size/2);
        const double D = CreateQuadtree(level-1, 4*index + 4, x + quadrant_size/2, y + quadrant_size/2);

        // Store quadrants in a quadtree
        // When level == 1, in the (n-1). recursion level a quadrant is sum of 4 pixels.

        const double quadrant_average_color = (A + B + C + D) / 4;

        // g_print("%f\n", quadrant_average_color);

        this->quadtree[index] = new Quadrant(quadrant_average_color);

        return quadrant_average_color;
    }

    // else if level == 0, the n. recursion level, there are pixels.
    const int rowstride = gdk_pixbuf_get_rowstride(this->pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(this->pixbuf);
    const int n_channels = gdk_pixbuf_get_n_channels(this->pixbuf);

    const guchar *p = pixels + x * rowstride + y * n_channels;
    const guchar r = p[0];
    const guchar g = p[1];
    const guchar b = p[2];

    //Do not store pixels, just the average.

    return (r + g + b) / 3; // Average color of the pixel
}

double Coding::round_to_decimal(double value, int decimal_places) {
    double factor = pow(10.0, decimal_places);
    return round(value * factor) / factor;
}

void Coding::ChildPointers() {
    for (int i = 0; i < this->quadtree_size_1; ++i) {

        int pi; // parent index
        int ai, bi, ci, di; // children indexes

        if(i < 5) {
            pi = 0;
        } else {
            pi = (i - 1) / 4;
        }
        if( i < this->quadtree_size_2 ) {
            ai = 4*i+1;
            bi = 4*i+2;
            ci = 4*i+3;
            di = 4*i+4;
        }
        else {
            ai = i;
            bi = i;
            ci = i;
            di = i;
        }
        this->quadtree[i]->parent = quadtree[pi];
        this->quadtree[i]->a = quadtree[ai];
        this->quadtree[i]->b = quadtree[bi];
        this->quadtree[i]->c = quadtree[ci];
        this->quadtree[i]->d = quadtree[di];
    }
}

void Coding::SaveWFA(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == nullptr) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "%d\n\n", static_cast<int>(State::states_count));

    for (size_t i = 0; i < this->F->size1; ++i) {
        if(i < this->F->size1 - 1) {
            fprintf(file, "%g ", gsl_matrix_get(F, i, 0));
        } else {
            fprintf(file, "%g", gsl_matrix_get(F, i, 0));
        }
    }

    fprintf(file, "\n\n");

    for (size_t i = 0; i < this->A->size1; ++i) {
        for (size_t j = 0; j < this->A->size2; ++j) {
            if(j < this->A->size2 - 1) {
                fprintf(file, "%g ", gsl_matrix_get(A, i, j));
            } else {
                fprintf(file, "%g", gsl_matrix_get(A, i, j));
            }
        }
        fprintf(file, "\n");
    }

    fprintf(file, "\n");

    for (size_t i = 0; i < this->B->size1; ++i) {
        for (size_t j = 0; j < this->B->size2; ++j) {
            if(j < this->B->size2 - 1) {
                fprintf(file, "%g ", gsl_matrix_get(B, i, j));
            } else {
                fprintf(file, "%g", gsl_matrix_get(B, i, j));
            }
        }
        fprintf(file, "\n");
    }

    fprintf(file, "\n");

    for (size_t i = 0; i < this->C->size1; ++i) {
        for (size_t j = 0; j < this->C->size2; ++j) {
            if(j < this->C->size2 - 1) {
                fprintf(file, "%g ", gsl_matrix_get(C, i, j));
            } else {
                fprintf(file, "%g", gsl_matrix_get(C, i, j));
            }
        }
        fprintf(file, "\n");
    }

    fprintf(file, "\n");

    for (size_t i = 0; i < this->D->size1; ++i) {
        for (size_t j = 0; j < this->D->size2; ++j) {
            if(j < this->D->size2 - 1) {
                fprintf(file, "%g ", gsl_matrix_get(D, i, j));
            } else {
                fprintf(file, "%g", gsl_matrix_get(D, i, j));
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}