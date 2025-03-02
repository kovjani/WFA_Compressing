#ifndef NONDETERMINISTICCODING_H
#define NONDETERMINISTICCODING_H

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/IterativeLinearSolvers>

#include "Coding.h"
#include "Transition.h"

using namespace std;
using namespace Eigen;

class NondeterministicCoding : public Coding {
private:
    MatrixXd A{}, B{}, C{}, D{};
    VectorXd F{};

    int number_of_states{0};

    VectorXd FindCoefficients(const VectorXd& phi);

protected:
    void CreateWFA() override;
    void ScanState(Quadrant &quadrant, char quadrant_symbol, int parent_state_index) override;
    void SaveWFA(const char *filename) override;

public:
    NondeterministicCoding() = default;
    NondeterministicCoding(const char *opened_filename, const char *saved_filename, int details, double epsilon, int number_of_states);
};

#endif //NONDETERMINISTICCODING_H