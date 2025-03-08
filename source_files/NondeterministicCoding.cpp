#include "../header_files/NondeterministicCoding.h"

#include <iomanip>

NondeterministicCoding::NondeterministicCoding(const char *opened_filename, const char *saved_filename, int details, double epsilon)
    : Coding(opened_filename, saved_filename, details, epsilon){
    A.setZero();
    B.setZero();
    C.setZero();
    D.setZero();
    M.setZero();
}

void NondeterministicCoding::CreateWFA() {

    // First state
    this->states[0] = this->quadtree[0];
    Quadrant *state_quadrants = new Quadrant[this->details];
    int index = 0;
    GetQuadrants(this->coding_depth, index, this->states[0], state_quadrants);
    for (int j = 0; j < this->details; j++) {
        M(j, 0) = state_quadrants[j].brightness;
    }
    delete[] state_quadrants;

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

    }
}

void NondeterministicCoding::ScanState(Quadrant &quadrant, char quadrant_symbol, int &parent_state_index) {

    VectorXd b(this->details);

    Quadrant *quadrantsb = new Quadrant[b.size()];
    int index = 0;
    GetQuadrants(this->coding_depth, index, quadrant, quadrantsb);

    for (int i = 0; i < b.size(); ++i) {
        b(i) = quadrantsb[i].brightness;
    }

    delete[] quadrantsb;

    // Solve the linear least squares problem using QR decomposition
    VectorXd X = M.colPivHouseholderQr().solve(b);

    RowVectorXd S(this->states_counter);
    for (int i = 0; i < this->states_counter; ++i) {
        S(i) = this->states[i].brightness;
    }

    // Check if quadrant can be created by linear combination of other states.
    if(fabs((S * X) - quadrant.brightness) > this->EPS) {
        // If not, create new state representing scaned quadrant.
        this->states_counter++;

        g_print("%d:%d\n", parent_state_index, this->states_counter);

        A.conservativeResize(this->states_counter, this->states_counter);
        A.rightCols(1).setZero();
        A.bottomRows(1).setZero();

        B.conservativeResize(this->states_counter, this->states_counter);
        B.rightCols(1).setZero();
        B.bottomRows(1).setZero();

        C.conservativeResize(this->states_counter, this->states_counter);
        C.rightCols(1).setZero();
        C.bottomRows(1).setZero();

        D.conservativeResize(this->states_counter, this->states_counter);
        D.rightCols(1).setZero();
        D.bottomRows(1).setZero();


        quadrant.state_index = this->states_counter - 1;
        this->states[this->states_counter - 1] = quadrant;


        M.conservativeResize(this->details, this->states_counter);
        Quadrant *state_quadrants = new Quadrant[this->details];
        index = 0;
        GetQuadrants(this->coding_depth, index, quadrant, state_quadrants);
        for (int j = 0; j < this->details; j++) {
            M(j, this->states_counter - 1) = state_quadrants[j].brightness;
        }
        delete[] state_quadrants;


        X.resize(this->states_counter);
        X.setZero();
        X(quadrant.state_index) = 1;
    }

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
        oss << fixed << setprecision(this->rounded) << this->states[i].brightness;
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
            oss << fixed << setprecision(this->rounded) << this->A(i, j);
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
            oss << fixed << setprecision(this->rounded) << this->B(i, j);
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
            oss << fixed << setprecision(this->rounded) << this->C(i, j);
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
            oss << fixed << setprecision(this->rounded) << this->D(i, j);
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