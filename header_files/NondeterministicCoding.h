#ifndef NONDETERMINISTICCODING_H
#define NONDETERMINISTICCODING_H

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/IterativeLinearSolvers>
#include <vector>

#include "Coding.h"
#include "Transition.h"

using namespace std;
using namespace Eigen;

class NondeterministicCoding : public Coding {
private:
    MatrixXd M{MatrixXd(this->details, this->states_counter)};

    MatrixXd A{MatrixXd(this->states_counter, this->states_counter)};
    MatrixXd B{MatrixXd(this->states_counter, this->states_counter)};
    MatrixXd C{MatrixXd(this->states_counter, this->states_counter)};
    MatrixXd D{MatrixXd(this->states_counter, this->states_counter)};

    void CreateWFA() override;
    void ScanState(Quadrant &quadrant, char quadrant_symbol, int &parent_state_index) override;
    void SaveWFA(const char *filename) override;

public:
    NondeterministicCoding() = default;
    NondeterministicCoding(const char *opened_filename, const char *saved_filename, int details, double epsilon);
};

#endif //NONDETERMINISTICCODING_H