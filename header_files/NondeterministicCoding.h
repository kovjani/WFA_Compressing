#ifndef NONDETERMINISTICCODING_H
#define NONDETERMINISTICCODING_H

#include <eigen3/Eigen/Dense>

#include "Coding.h"

using namespace std;
using namespace Eigen;

class NondeterministicCoding : public Coding {
private:

    int coding_depth{0};
    int details{0};

    MatrixXd M{}, A{}, B{}, C{}, D{};

    void CreateWFA() override;
    void ScanState(Quadrant &quadrant, char quadrant_symbol, int &parent_state_index) override;
    void SaveWFA(const char *filename) override;

public:
    NondeterministicCoding() = default;
    NondeterministicCoding(const char *opened_filename, const char *saved_filename, int coding_depth);
};

#endif //NONDETERMINISTICCODING_H