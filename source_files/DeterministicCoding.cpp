#include "../header_files/DeterministicCoding.h"

DeterministicCoding::DeterministicCoding(const char *opened_filename, const char *saved_filename)
    : Coding(opened_filename, saved_filename) {

    this->EPS = 0.0001;

    this->A = new Transition [this->quadtree_size];
    this->B = new Transition [this->quadtree_size];
    this->C = new Transition [this->quadtree_size];
    this->D = new Transition [this->quadtree_size];
}

DeterministicCoding::~DeterministicCoding() {
    // Free the allocated memory.
    delete[] this->A;
    this->A = nullptr;

    delete[] this->B;
    this->B = nullptr;

    delete[] this->C;
    this->C = nullptr;

    delete[] this->D;
    this->D = nullptr;

    delete[] this->states;
    this->states = nullptr;
}

void DeterministicCoding::CreateWFA() {

    this->quadtree[0].state_index = 0;
    this->states[0] = quadtree[0];

    for (int i = 0; i < this->states_counter; ++i) {

        Quadrant scanned_state = this->states[i];

        Quadrant a = *scanned_state.a;
        Quadrant b = *scanned_state.b;
        Quadrant c = *scanned_state.c;
        Quadrant d = *scanned_state.d;

        ScanState(a, 'a', i);
        ScanState(b, 'b', i);
        ScanState(c, 'c', i);
        ScanState(d, 'd', i);

        g_print("%d:%d\n", i, this->states_counter);

    }
}

void DeterministicCoding::ScanState(Quadrant &quadrant, char quadrant_symbol, int &parent_state_index) {

    for (int i = 0; i < this->states_counter; ++i) {
        // state image vs quadrant
        Quadrant state_image = this->states[i];

        // Compare scanned quadrant's children and appropriate grandchildren of a parent.
        double cost = CompareQuadrants(quadrant, state_image);

        // If scanned state does not represent the quadrant with any x, but maybe another state.
        if( cost == -1 ) {
            continue;
        }

        // If an x cost found, add new transition into the appropriate matrix.

        int scanned_state_index = state_image.state_index;

        switch (quadrant_symbol) {
            case 'a':
                this->A[parent_state_index] = Transition(parent_state_index, scanned_state_index, cost);
            break;
            case 'b':
                this->B[parent_state_index] = Transition(parent_state_index, scanned_state_index, cost);
            break;
            case 'c':
                this->C[parent_state_index] = Transition(parent_state_index, scanned_state_index, cost);
            break;
            case 'd':
                this->D[parent_state_index] = Transition(parent_state_index, scanned_state_index, cost);
            break;
        }
        return;     // Do not create new state and not scan other states.
    }

    // Create new state

    int new_state_index = this->states_counter;

    quadrant.state_index = new_state_index;

    this->states[this->states_counter++] = quadrant;

    //Set transition to 1
    switch (quadrant_symbol) {
        case 'a':
            this->A[parent_state_index] = Transition(parent_state_index, new_state_index, 1);
        break;
        case 'b':
            this->B[parent_state_index] = Transition(parent_state_index, new_state_index, 1);
        break;
        case 'c':
            this->C[parent_state_index] = Transition(parent_state_index, new_state_index, 1);
        break;
        case 'd':
            this->D[parent_state_index] = Transition(parent_state_index, new_state_index, 1);
        break;
    }
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
        oss << fixed << setprecision(this->rounded) << this->states[i].brightness;
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
        if(this->A[i].value == 1) {
            if( i < this->states_counter - 1 ) {
                file << this->A[i].j << endl;
            } else {
                file << this->A[i].j << endl << endl;
            }
            continue;
        }

        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(this->rounded) << this->A[i].value;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << this->A[i].j << " " << brightness << endl;
        } else {
            file << this->A[i].j << " " << brightness << endl << endl;
        }
    }

    // B
    for (int i = 0; i < this->states_counter; ++i) {
        // If the cost is 1, don't store it, only j. state enough, because there might be a lot of 1 cost in the automata.
        // So if there is no cost in a line, the decoding algorithm uses 1.
        if(this->B[i].value == 1) {
            if( i < this->states_counter - 1 ) {
                file << this->B[i].j << endl;
            } else {
                file << this->B[i].j << endl << endl;
            }
            continue;
        }

        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(this->rounded) << this->B[i].value;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << this->B[i].j << " " << brightness << endl;
        } else {
            file << this->B[i].j << " " << brightness << endl << endl;
        }
    }

    // C
    for (int i = 0; i < this->states_counter; ++i) {
        // If the cost is 1, don't store it, only j. state enough, because there might be a lot of 1 cost in the automata.
        // So if there is no cost in a line, the decoding algorithm uses 1.
        if(this->C[i].value == 1) {
            if( i < this->states_counter - 1 ) {
                file << this->C[i].j << endl;
            } else {
                file << this->C[i].j << endl << endl;
            }
            continue;
        }

        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(this->rounded) << this->C[i].value;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << this->C[i].j << " " << brightness << endl;
        } else {
            file << this->C[i].j << " " << brightness << endl << endl;
        }
    }

    // D
    for (int i = 0; i < this->states_counter; ++i) {
        // If the cost is 1, don't store it, only j. state enough, because there might be a lot of 1 cost in the automata.
        // So if there is no cost in a line, the decoding algorithm uses 1.
        if(this->D[i].value == 1) {
            if( i < this->states_counter - 1 ) {
                file << this->D[i].j << endl;
            } else {
                file << this->D[i].j << endl << endl;
            }
            continue;
        }

        // Round value to 4 decimals
        ostringstream oss;
        oss << fixed << setprecision(this->rounded) << this->D[i].value;
        std::string brightness = oss.str();

        // Remove trailing zeros
        brightness.erase(brightness.find_last_not_of('0') + 1, std::string::npos);
        if (brightness.back() == '.') {
            brightness.pop_back();
        }

        if( i < this->states_counter - 1 ) {
            file << this->D[i].j << " " << brightness << endl;
        } else {
            file << this->D[i].j << " " << brightness << endl << endl;
        }
    }
}