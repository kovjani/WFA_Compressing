#include "../header_files/NondeterministicCoding.h"

#include <iomanip>

NondeterministicCoding::NondeterministicCoding(const char *filename, double epsilon, int number_of_states, int details, char *saved_filename) {

    this->number_of_states = number_of_states;
    this->details = details;
    this->directory = g_path_get_dirname(filename);
    this->saved_filename = saved_filename;

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
    for (int i = 0; i < this->depth; ++i) {
        this->quadtree_size += pow(4, i);
    }

    /*g_print("%d\n", this->coding_image_size);
    g_print("%d\n", this->quadtree_size);*/

    this->quadtree = new Quadrant[this->quadtree_size];
    // The maximum number of states is the number of quadtree nodes.
    this->states = new Quadrant [this->quadtree_size];
}

NondeterministicCoding::~NondeterministicCoding() {
    // Free the allocated memory.
    g_object_unref(this->pixbuf);

    /*for (int i = 0; i < this->quadtree_size; ++i) {
        delete this->quadtree[i];
        this->quadtree[i] = nullptr;
    }*/

    delete[] this->quadtree;
    this->quadtree = nullptr;

    delete[] this->states;
    this->states = nullptr;
}

const Quadrant& NondeterministicCoding::operator[](unsigned i) const {
    double j = i;
    while(j >= this->quadtree_size) {
        j /= 4;
    }
    i = static_cast<int>(floor(j));
    return this->quadtree[i];
}

void NondeterministicCoding::Start() {
    // CreateBinarytree(this->depth, 0, 0, 0);
    CreateQuadtree(this->depth, 0, 0, 0);
    // ChildPointers();

    for (int i = 0; i < this->quadtree_size; i++) {
       // g_print("%f ", this->quadtree[i]->brightness);
    }

    CreateWFA();

    char *filename_with_path = g_build_filename(this->directory, saved_filename, nullptr);

    g_print("%s ", filename_with_path);

    SaveWFA(filename_with_path);

    g_free(filename_with_path);

    //delete pic;
    //pic = nullptr;
}

void NondeterministicCoding::CreateWFA() {

    this->A.resize(this->number_of_states, this->number_of_states);
    this->B.resize(this->number_of_states, this->number_of_states);
    this->C.resize(this->number_of_states, this->number_of_states);
    this->D.resize(this->number_of_states, this->number_of_states);

    this->F.resize(this->number_of_states);

    this->A.setZero();
    this->B.setZero();
    this->C.setZero();
    this->D.setZero();

    this->F.setZero();

    for (int i = 0; this->states_counter < this->number_of_states && i < this->number_of_states * 2; ++i) {
        /*bool state_found = false;
        for (int j = 0; j < this->states_counter; ++j) {
            double result = CompareQuadrants(this->quadtree[i], this->states[j]);

            if(result != -1) {
            //if(fabs(this->quadtree[i]->brightness - this->states[j]->brightness) < 0.01) {
                state_found = true;
                break;
            }
        }*/
       // if(!state_found) {
            this->states[this->states_counter] = this->quadtree[i];
            this->F(this->states_counter) = this->quadtree[i].brightness;
            // g_print("%f ", F(this->states_counter));
            this->states_counter++;
        //}
    }

    if(this->number_of_states != this->states_counter){
        this->states_counter = this->number_of_states;
        for (int i = 0; i < this->number_of_states; ++i) {
            this->states[i] = this->quadtree[i];
            this->F(i) = this->quadtree[i].brightness;
        }
    }

    for (int i = 0; i < this->number_of_states; ++i) {

        Quadrant scanned_state = this->states[i];

        if(scanned_state.a == nullptr || scanned_state.a->a == nullptr) break;

       // g_print("%f ", scanned_state->brightness);

        ScanState(*scanned_state.a, 'a', i);
        ScanState(*scanned_state.b, 'b', i);
        ScanState(*scanned_state.c, 'c', i);
        ScanState(*scanned_state.d, 'd', i);

    }
}

VectorXd NondeterministicCoding::FindCoefficients(const VectorXd& phi) {
    MatrixXd A(phi.size(), this->number_of_states);

    for (int i = 0; i < this->number_of_states; ++i) {
        int state = this->states[i].index;
        for (int j = 0; j < phi.size(); j++) {
            A(j, i) = (*this)[phi.size() * state + j + 1].brightness;
        }
    }

    // Solve the linear least squares problem with L2 regularization
    MatrixXd ATA = A.transpose() * A;
    VectorXd ATphi = A.transpose() * phi;
    VectorXd coefficients = ATA.ldlt().solve(ATphi);

    return coefficients;
}

void NondeterministicCoding::ScanState(Quadrant &quadrant, char quadrant_symbol, int parent_state_index) {

    VectorXd phi(this->details);
    for (int i = 0; i < phi.size(); ++i) {
        phi(i) = (*this)[phi.size() * quadrant.index + i + 1].brightness;
    }

    VectorXd X = FindCoefficients(phi);

    switch (quadrant_symbol) {
        case 'a':
            this->A.row(parent_state_index) = X;
        break;
        case 'b':
            this->B.row(parent_state_index) = X;
        break;
        case 'c':
            this->C.row(parent_state_index) = X;
        break;
        case 'd':
            this->D.row(parent_state_index) = X;
        break;
    }
}


double NondeterministicCoding::CompareQuadrants(const Quadrant &q1, const Quadrant &q2) const {

    // q1: quadrant
    // q2: state image

    // If q1 or q2 is a pixel.
    if(q1.a == nullptr || q2.a == nullptr) {
        return q1.brightness / q2.brightness;
    }

    double cost_a = q1.a->brightness / q2.a->brightness;
    double cost_b = q1.b->brightness / q2.b->brightness;
    double cost_c = q1.c->brightness / q2.c->brightness;
    double cost_d = q1.d->brightness / q2.d->brightness;

   // g_print("%f %f %f %f\n", cost_a, cost_b, cost_c, q2.d->brightness);

    // If costs are equal for all nodes according to epsilon, return cost else return -1
    if( fabs(cost_a - cost_b) <= this->EPS && fabs(cost_a - cost_c) <= this->EPS && fabs(cost_a - cost_d) <= this->EPS  ){
        return cost_a;
    }

    return -1; // images are not the same
}

Quadrant* NondeterministicCoding::CreateQuadtree(int level, int index, int x, int y){
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

        this->quadtree[index] = Quadrant(quadrant_average_color);
        this->quadtree[index].index = index;

        // quadtree level
        this->quadtree[index].level = level;

        this->quadtree[index].a = A;
        this->quadtree[index].b = B;
        this->quadtree[index].c = C;
        this->quadtree[index].d = D;

        this->quadtree[index].a->parent = &this->quadtree[index];
        this->quadtree[index].b->parent = &this->quadtree[index];
        this->quadtree[index].c->parent = &this->quadtree[index];
        this->quadtree[index].d->parent = &this->quadtree[index];

        return &this->quadtree[index];
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
    double brightness = (static_cast<double>(r + g + b) / 3) / 255;
    return new Quadrant(fabs(brightness) < 0.0001 ? 0.0001 : brightness);
}

void NondeterministicCoding::SaveWFA(const char *filename) {
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
        oss << fixed << setprecision(4) << this->states[i].brightness;
        //oss << fixed << setprecision(4) << this->quadtree[i]->brightness;
        std::string str = oss.str();

        // Remove trailing zeros
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') {
            str.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << str << " ";
        } else {
            file << str << endl << endl;
        }
    }

    // A
    for (int i = 0; i < this->states_counter; ++i) {
        for (int j = 0; j < this->states_counter; ++j) {

            // Round value to 4 decimals
            ostringstream oss;
            oss << fixed << setprecision(4) << this->A(i, j);
            std::string str = oss.str();

            // Remove trailing zeros
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.') {
                str.pop_back();
            }

            if( j < this->states_counter - 1 ) {
                file << str << " ";
            } else {
                file << str << endl;
            }
        }
    }

    file << endl;

    // B
    for (int i = 0; i < this->states_counter; ++i) {
        for (int j = 0; j < this->states_counter; ++j) {

            // Round value to 4 decimals
            ostringstream oss;
            oss << fixed << setprecision(4) << this->B(i, j);
            std::string str = oss.str();

            // Remove trailing zeros
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.') {
                str.pop_back();
            }

            if( j < this->states_counter - 1 ) {
                file << str << " ";
            } else {
                file << str << endl;
            }
        }
    }

    file << endl;

    // C
    for (int i = 0; i < this->states_counter; ++i) {
        for (int j = 0; j < this->states_counter; ++j) {

            // Round value to 4 decimals
            ostringstream oss;
            oss << fixed << setprecision(4) << this->C(i, j);
            std::string str = oss.str();

            // Remove trailing zeros
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.') {
                str.pop_back();
            }

            if( j < this->states_counter - 1 ) {
                file << str << " ";
            } else {
                file << str << endl;
            }
        }
    }

    file << endl;

    // D
    for (int i = 0; i < this->states_counter; ++i) {
        for (int j = 0; j < this->states_counter; ++j) {

            // Round value to 4 decimals
            ostringstream oss;
            oss << fixed << setprecision(4) << this->D(i, j);
            std::string str = oss.str();

            // Remove trailing zeros
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.') {
                str.pop_back();
            }

            if( j < this->states_counter - 1 ) {
                file << str << " ";
            } else {
                file << str << endl;
            }
        }
    }

    file.close();
}