#include "../header_files/DeterministicCoding.h"

DeterministicCoding::DeterministicCoding(const char *filename, double epsilon) {
    this->EPS = epsilon;

    // Open the image and split it into pieces, than store it in a quadtree.
    // It helps to create the automaton.

    // Open image
    this->pixbuf = gdk_pixbuf_new_from_file(filename, &this->error);

    // Compare pixel colors and create automata.

    this->width = gdk_pixbuf_get_width(this->pixbuf);
    this->height = gdk_pixbuf_get_height(this->pixbuf);
    this->coding_image_size = this->width > this->height ? this->height : this->width;

    // A full quadtree for a (2^level*2^level) image has ((4^level - 1) / 3) nodes.
    // The number of leaves in a full quadtree is (4^level).
    // We don't want to store the leaves.
    this->depth = static_cast <int> (log2(this->coding_image_size)); // maximum recursion level
    this->quadtree_size = 0;

    // In this iteration the maximum value of i is depth-1
    for (int i = 0; i < this->depth; ++i) {
        this->quadtree_size += pow(4, i);
    }

    this->quadtree = new Quadrant*[this->quadtree_size];

    // The maximum number of states is the number of quadtree nodes.
    this->states = new Quadrant *[this->quadtree_size];
    this->sorted_states = new Quadrant *[this->quadtree_size];

    this->A = new Transition *[this->quadtree_size];
    this->B = new Transition *[this->quadtree_size];
    this->C = new Transition *[this->quadtree_size];
    this->D = new Transition *[this->quadtree_size];
}

DeterministicCoding::~DeterministicCoding() {
    // Free the allocated memory.

    g_object_unref(this->pixbuf);

    for (int i = 0; i < this->quadtree_size; ++i) {
        delete this->quadtree[i];
        this->quadtree[i] = nullptr;
    }

    for (int i = 0; i < this->states_counter; ++i) {
        // Elements of states and sorted_states are elements of quadtree aswell, so they have been deleted above.

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

    delete[] this->sorted_states;
    this->sorted_states = nullptr;

    delete[] this->A;
    this->A = nullptr;

    delete[] this->B;
    this->B = nullptr;

    delete[] this->C;
    this->C = nullptr;

    delete[] this->D;
    this->D = nullptr;
}

void DeterministicCoding::Start() {

    CreateQuadtree(this->depth, 0, 0, 0);

    CreateWFA();

    SaveWFA("/home/jani/wfa.wfa");

}

void DeterministicCoding::CreateWFA() {

    this->quadtree[0]->index = 0;
    this->states[this->states_counter++] = quadtree[0];

    for (int i = 0; i < this->states_counter; ++i) {

        Quadrant *scanned_state = this->states[i];

        if(scanned_state->a == nullptr) break;

        ScanState(*scanned_state->a, 'a');
        ScanState(*scanned_state->b, 'b');
        ScanState(*scanned_state->c, 'c');
        ScanState(*scanned_state->d, 'd');

       // if( i % 100 == 2)
            g_print("%d\n", this->states_counter);
    }
}

void DeterministicCoding::ScanState(Quadrant &quadrant, char quadrant_symbol) {

    for (int i = 0; i < this->states_counter; ++i) {
         /*this->calling_counter++;
        if(this->calling_counter % 100000 == 0) {
            // Process GTK events to keep the UI responsive
            while (gtk_events_pending())
                gtk_main_iteration();
            g_print("%d\n", this->states_counter);
        }*/

        // state image vs quadrant
        Quadrant *state_image = this->states[i];

        // Compare scanned quadrant's children and appropriate grandchildren of a parent.
        double cost = CompareQuadrants(quadrant, *state_image);

        // If scanned state does not represent the quadrant with any x, but maybe another state.
        if( cost == -1 ) {
            continue;
        }

        // If an x cost found, add new transition into the appropriate matrix.

        int parent_state_index = quadrant.parent->index;
        int scanned_state_index = state_image->index;

        switch (quadrant_symbol) {
            case 'a':
                this->A[parent_state_index] = new Transition(parent_state_index, scanned_state_index, cost);
            break;
            case 'b':
                this->B[parent_state_index] = new Transition(parent_state_index, scanned_state_index, cost);
            break;
            case 'c':
                this->C[parent_state_index] = new Transition(parent_state_index, scanned_state_index, cost);
            break;
            case 'd':
                this->D[parent_state_index] = new Transition(parent_state_index, scanned_state_index, cost);
            break;
        }
        return;     // Do not create new state and not scan other states.
    }

    // Create new state

    int parent_state_index = quadrant.parent->index;
    int new_state_index = this->states_counter;

    quadrant.index = new_state_index;

    this->states[this->states_counter++] = &quadrant;

    //Set transition to 1
    switch (quadrant_symbol) {
        case 'a':
            this->A[parent_state_index] = new Transition(parent_state_index, new_state_index, 1);
        break;
        case 'b':
            this->B[parent_state_index] = new Transition(parent_state_index, new_state_index, 1);
        break;
        case 'c':
            this->C[parent_state_index] = new Transition(parent_state_index, new_state_index, 1);
        break;
        case 'd':
            this->D[parent_state_index] = new Transition(parent_state_index, new_state_index, 1);
        break;
    }
}

double DeterministicCoding::CompareQuadrants(const Quadrant &q1, const Quadrant &q2) const {

    // q1: quadrant
    // q2: state image

    // If q1 or q2 is a pixel.
    if(q1.a == nullptr || q2.a == nullptr) {
        return q1.brightness / q2.brightness;
    }

    // If brightnesses of quadrants are equals, check children
    double cost_a = q1.a->brightness / q2.a->brightness;
    double cost_b = q1.b->brightness / q2.b->brightness;
    double cost_c = q1.c->brightness / q2.c->brightness;
    double cost_d = q1.d->brightness / q2.d->brightness;

    // If costs are equal for all nodes according to epsilon, return cost else return -1
    if( fabs(cost_a - cost_b) <= this->EPS && fabs(cost_a - cost_c) <= this->EPS && fabs(cost_a - cost_d) <= this->EPS  ){
        return cost_a;
    }

    return -1;
}

Quadrant *DeterministicCoding::CreateQuadtree(int level, int index, int x, int y){
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

        this->quadtree[index]->a = A;
        this->quadtree[index]->b = B;
        this->quadtree[index]->c = C;
        this->quadtree[index]->d = D;

        this->quadtree[index]->a->parent = this->quadtree[index];
        this->quadtree[index]->b->parent = this->quadtree[index];
        this->quadtree[index]->c->parent = this->quadtree[index];
        this->quadtree[index]->d->parent = this->quadtree[index];

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
    return new Quadrant((static_cast<double>(r + g + b) / 3) / 255);
}

void DeterministicCoding::SaveWFA(const char *filename) {
    ofstream file(filename);
    if(! file.is_open()) {
        g_printerr("error file opening");
        return;
    }

    file << this->states_counter << endl << endl;

    // F
    for (int i = 0; i < this->states_counter; ++i) {
        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(4) << this->states[i]->brightness;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << brightness << " ";
        } else {
            file << brightness << endl << endl;
        }
    }

    // A
    for (int i = 0; i < this->states_counter; ++i) {
        // If the cost is 1, don't store it, only j. state enough, because there might be a lot of 1 cost in the automata.
        // So if there is no cost in a line, the decoding algorithm uses 1.
        if(this->A[i]->value == 1) {
            if( i < this->states_counter - 1 ) {
                file << this->A[i]->j << endl;
            } else {
                file << this->A[i]->j << endl << endl;
            }
            continue;
        }

        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(4) << this->A[i]->value;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << this->A[i]->j << " " << brightness << endl;
        } else {
            file << this->A[i]->j << " " << brightness << endl << endl;
        }
    }

    // B
    for (int i = 0; i < this->states_counter; ++i) {
        // If the cost is 1, don't store it, only j. state enough, because there might be a lot of 1 cost in the automata.
        // So if there is no cost in a line, the decoding algorithm uses 1.
        if(this->B[i]->value == 1) {
            if( i < this->states_counter - 1 ) {
                file << this->B[i]->j << endl;
            } else {
                file << this->B[i]->j << endl << endl;
            }
            continue;
        }

        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(4) << this->B[i]->value;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << this->B[i]->j << " " << brightness << endl;
        } else {
            file << this->B[i]->j << " " << brightness << endl << endl;
        }
    }

    // C
    for (int i = 0; i < this->states_counter; ++i) {
        // If the cost is 1, don't store it, only j. state enough, because there might be a lot of 1 cost in the automata.
        // So if there is no cost in a line, the decoding algorithm uses 1.
        if(this->C[i]->value == 1) {
            if( i < this->states_counter - 1 ) {
                file << this->C[i]->j << endl;
            } else {
                file << this->C[i]->j << endl << endl;
            }
            continue;
        }

        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(4) << this->C[i]->value;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << this->C[i]->j << " " << brightness << endl;
        } else {
            file << this->C[i]->j << " " << brightness << endl << endl;
        }
    }

    // D
    for (int i = 0; i < this->states_counter; ++i) {
        // If the cost is 1, don't store it, only j. state enough, because there might be a lot of 1 cost in the automata.
        // So if there is no cost in a line, the decoding algorithm uses 1.
        if(this->D[i]->value == 1) {
            if( i < this->states_counter - 1 ) {
                file << this->D[i]->j << endl;
            } else {
                file << this->D[i]->j << endl << endl;
            }
            continue;
        }

        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(4) << this->D[i]->value;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << this->D[i]->j << " " << brightness << endl;
        } else {
            file << this->D[i]->j << " " << brightness << endl << endl;
        }
    }
}