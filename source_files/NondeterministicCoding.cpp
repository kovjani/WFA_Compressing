#include "../header_files/NondeterministicCoding.h"

#include <iomanip>

NondeterministicCoding::NondeterministicCoding(const char *opened_filename, const char *saved_filename, int details, double epsilon, int number_of_states)
    : Coding(opened_filename, saved_filename, details, epsilon){

    this->number_of_states = number_of_states;
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

        int si = scanned_state.quadtree_index;

        Quadrant a = (*this)[4*si + 1];
        Quadrant b = (*this)[4*si + 2];
        Quadrant c = (*this)[4*si + 3];
        Quadrant d = (*this)[4*si + 4];

        ScanState(a, 'a', i);
        ScanState(b, 'b', i);
        ScanState(c, 'c', i);
        ScanState(d, 'd', i);

    }
}

VectorXd NondeterministicCoding::FindCoefficients(const VectorXd& phi) {
    MatrixXd A(phi.size(), this->number_of_states);

    for (int i = 0; i < this->number_of_states; ++i) {
        int state = this->states[i].quadtree_index;
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
        phi(i) = (*this)[phi.size() * quadrant.quadtree_index + i + 1].brightness;
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