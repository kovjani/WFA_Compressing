#include "../header_files/Coding.h"

Coding::Coding(const char *filename, double epsilon) {
    this->EPS = epsilon;

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
    this->quadtree_size = 0;

    // In this iteration the maximum value of i is depth-1
    for (int i = 0; i <= this->depth; ++i) {
        this->quadtree_size += pow(4, i);
    }

    g_print("%d\n", this->coding_image_size);
    g_print("%d\n", this->quadtree_size);

    this->quadtree = new Quadrant*[this->quadtree_size];
    // The maximum number of states is the number of quadtree nodes.
    this->states = new Quadrant *[this->quadtree_size];

    this->A = new Element *[this->quadtree_size];
    this->B = new Element *[this->quadtree_size];
    this->C = new Element *[this->quadtree_size];
    this->D = new Element *[this->quadtree_size];
}

Coding::~Coding() {
    // Free the allocated memory.
    g_object_unref(this->pixbuf);

    for (int i = 0; i < this->quadtree_size; ++i) {
        delete this->quadtree[i];
        this->quadtree[i] = nullptr;
    }

    for (int i = 0; i < this->states_counter; ++i) {
        // Elements of states are elements of quadtree aswell, so they have been deleted above.
        /*delete this->states[i];
        this->states[i] = nullptr;*/

        delete this->A[i];
        this->A[i] = nullptr;

        delete this->B[i];
        this->B[i] = nullptr;

        delete this->C[i];
        this->C[i] = nullptr;

        delete this->D[i];
        this->D[i] = nullptr;
    }

    delete[] this->quadtree;
    this->quadtree = nullptr;

    delete[] this->states;
    this->states = nullptr;

    delete[] this->A;
    this->A = nullptr;

    delete[] this->B;
    this->B = nullptr;

    delete[] this->C;
    this->C = nullptr;

    delete[] this->D;
    this->D = nullptr;
}

void Coding::Start() {
    Quadrant *pic = CreateQuadtree(this->depth, 0, 0, 0);

   // ChildPointers();

    /*for (int i = 0; i < this->quadtree_size; i++) {
        g_print("%f ", this->quadtree[i]->brightness);
    }*/

    CreateWFA();

    SaveWFA("/home/jani/wfa.wfa");

    //delete pic;
    //pic = nullptr;
}

void Coding::CreateWFA() {

    // First state represents the whole image.
    /*if(State::states_counter == 0) {
        this->states = new State*[1];
        //Index is 0 by default
        this->states[0] = new State(this->quadtree[0]);
    }*/

    this->quadtree[0]->index = 0;
    this->states[this->states_counter++] = quadtree[0];

    for (int i = 0; i < this->states_counter; ++i) {

        Quadrant *scanned_state = this->states[i];

        // ???
        /*thread t1(&Coding::ScanState, this, scanned_state->a, 'a', i);
        t1.join();
        thread t2(&Coding::ScanState, this, scanned_state->b, 'b', i);*/

        ScanState(*scanned_state->a, 'a', 0, &i);
        ScanState(*scanned_state->b, 'b', 0, &i);
        ScanState(*scanned_state->c, 'c', 0, &i);
        ScanState(*scanned_state->d, 'd', 0, &i);
    }
}


void Coding::ScanState(Quadrant &quadrant, char quadrant_symbol, int state_index, int *quadrant_index) {

    this->calling_counter++;
    if(this->calling_counter % 100000 == 0) {
        // Process GTK events to keep the UI responsive
        while (gtk_events_pending())
            gtk_main_iteration();
        g_print("%d\n", this->states_counter);
    }

    // state image vs quadrant

    Quadrant *state_image = this->states[state_index];

    // Compare scanned quadrant's children and appropriate grandchildren of a parent.
    // quadrant vs state_image

    double cost = CompareQuadrants(quadrant, *state_image);
    if( cost != -1 ) {

        int parent_state_index = quadrant.parent->index;
        int scanned_state_index = state_image->index;

        // If costs are equal add new transition into the appropriate matrix.

        switch (quadrant_symbol) {
            case 'a':
                this->A[parent_state_index] = new Element(parent_state_index, scanned_state_index, cost);
            break;
            case 'b':
                this->B[parent_state_index] = new Element(parent_state_index, scanned_state_index, cost);
            break;
            case 'c':
                this->C[parent_state_index] = new Element(parent_state_index, scanned_state_index, cost);
            break;
            case 'd':
                this->D[parent_state_index] = new Element(parent_state_index, scanned_state_index, cost);
            break;
        }
    } else if(state_index < this->states_counter - 1) {

        // If not, check other states
        ScanState(quadrant, quadrant_symbol, state_index + 1, quadrant_index);

    } else {
        // Create new state

        //*quadrant_index = 0;

        int parent_state_index = quadrant.parent->index;
        int new_state_index = this->states_counter;

        quadrant.index = new_state_index;

        this->states[this->states_counter++] = &quadrant;

       // this->F.push_back(new Element(quadrant->brightness));

        //Set transition to 1
        switch (quadrant_symbol) {
            case 'a':
                this->A[parent_state_index] = new Element(parent_state_index, new_state_index, 1);
            break;
            case 'b':
                this->B[parent_state_index] = new Element(parent_state_index, new_state_index, 1);
            break;
            case 'c':
                this->C[parent_state_index] = new Element(parent_state_index, new_state_index, 1);
            break;
            case 'd':
                this->D[parent_state_index] = new Element(parent_state_index, new_state_index, 1);
            break;
        }
    }
}

double Coding::CompareQuadrants(const Quadrant &q1, const Quadrant &q2) const {
    double cost_a = q1.a->brightness / q2.a->brightness;
    double cost_b = q1.b->brightness / q2.b->brightness;
    double cost_c = q1.c->brightness / q2.c->brightness;
    double cost_d = q1.d->brightness / q2.d->brightness;


    // If costs are equal for all nodes according to epsilon, return cost else return -1
    if( abs(cost_a - cost_b) < this->EPS && abs(cost_a - cost_c) < this->EPS && abs(cost_a - cost_d) < this->EPS  ){
        return cost_a;
    }
    return -1;
}


Quadrant *Coding::CreateQuadtree(int level, int index, int x, int y){
    // Count the average color of the image segment (quadrant) according to parameters.

    if(level > 0) {

        const int quadrant_size = 1 << level; // 2^level
        // The average colors of quadrants
        Quadrant *A = CreateQuadtree(level-1, 4*index + 1, x, y);
        Quadrant *B = CreateQuadtree(level-1, 4*index + 2, x + quadrant_size/2, y);
        Quadrant *C = CreateQuadtree(level-1, 4*index + 3, x, y + quadrant_size/2);
        Quadrant *D = CreateQuadtree(level-1, 4*index + 4, x + quadrant_size/2, y + quadrant_size/2);

        // Store quadrants in a quadtree
        // When level == 1, in the (n-1). recursion level a quadrant is sum of 4 pixels.

        const double quadrant_average_color = (A->brightness + B->brightness + C->brightness + D->brightness) / 4;

        // g_print("%f\n", quadrant_average_color);

        this->quadtree[index] = new Quadrant(quadrant_average_color);

        if(level == this->depth) {
            // The parent of root is a pointer to itself.
            this->quadtree[index]->parent = this->quadtree[index];
        }

        this->quadtree[index]->a = A;
        this->quadtree[index]->b = B;
        this->quadtree[index]->c = C;
        this->quadtree[index]->d = D;

        this->quadtree[index]->a->parent = this->quadtree[index];
        this->quadtree[index]->b->parent = this->quadtree[index];
        this->quadtree[index]->c->parent = this->quadtree[index];
        this->quadtree[index]->d->parent = this->quadtree[index];

        // The children of leaves are pointers to itself.
        if(level == 1) {
            this->quadtree[index]->a->a = this->quadtree[index]->a;
            this->quadtree[index]->a->b = this->quadtree[index]->a;
            this->quadtree[index]->a->c = this->quadtree[index]->a;
            this->quadtree[index]->a->d = this->quadtree[index]->a;

            this->quadtree[index]->b->a = this->quadtree[index]->b;
            this->quadtree[index]->b->b = this->quadtree[index]->b;
            this->quadtree[index]->b->c = this->quadtree[index]->b;
            this->quadtree[index]->b->d = this->quadtree[index]->b;

            this->quadtree[index]->c->a = this->quadtree[index]->c;
            this->quadtree[index]->c->b = this->quadtree[index]->c;
            this->quadtree[index]->c->c = this->quadtree[index]->c;
            this->quadtree[index]->c->d = this->quadtree[index]->c;

            this->quadtree[index]->d->a = this->quadtree[index]->d;
            this->quadtree[index]->d->b = this->quadtree[index]->d;
            this->quadtree[index]->d->c = this->quadtree[index]->d;
            this->quadtree[index]->d->d = this->quadtree[index]->d;
        }

        return this->quadtree[index];
    }

    // else if level == 0, the n. recursion level, there are pixels.
    const int rowstride = gdk_pixbuf_get_rowstride(this->pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(this->pixbuf);
    const int n_channels = gdk_pixbuf_get_n_channels(this->pixbuf);

    const guchar *p = pixels + x * rowstride + y * n_channels;
    const int r = p[0];
    const int g = p[1];
    const int b = p[2];

    //Do not store pixels as quadrants, just the average color of 4 pixel.
    return new Quadrant(static_cast<double>(r + g + b) / 3);
}

void Coding::SaveWFA(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == nullptr) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "%d\n\n", this->states_counter);

    // F
    for (size_t i = 0; i < this->states_counter; ++i) {
        if(i < this->states_counter - 1) {
            fprintf(file, "%.4g ", this->states[i]->brightness/255);
        } else {
            fprintf(file, "%.4g\n", this->states[i]->brightness/255);
        }
    }

    fprintf(file, "\n");

    // A
    for (size_t i = 0; i < this->states_counter; ++i) {
        fprintf(file, "%d %.4g\n", this->A[i]->j, this->A[i]->value);
    }

    fprintf(file, "\n");

    // B
    for (size_t i = 0; i < this->states_counter; ++i) {
        fprintf(file, "%d %.4g\n", this->B[i]->j, this->B[i]->value);
    }

    fprintf(file, "\n");

    // C
    for (size_t i = 0; i < this->states_counter; ++i) {
        fprintf(file, "%d %.4g\n", this->C[i]->j, this->C[i]->value);
    }

    fprintf(file, "\n");

    // D
    for (size_t i = 0; i < this->states_counter; ++i) {
        fprintf(file, "%d %.4g\n", this->D[i]->j, this->D[i]->value);
    }

    fclose(file);
}