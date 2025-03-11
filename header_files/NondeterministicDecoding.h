#ifndef NONDETERMINISTICDECODING_H
#define NONDETERMINISTICDECODING_H

#include <eigen3/Eigen/Dense>

#include "Decoding.h"

using namespace std;
using namespace Eigen;

class NondeterministicDecoding : public Decoding {
private:
    //Inn is an nxn identity matrix for the first call.
    MatrixXd A{}, B{}, C{}, D{}, Inn{};
    RowVectorXd I{};
    VectorXd F{};

    int n{};  // Number of states.

    // Same name but different params                                                                   here
    void DecodePixelsColors(int level, int x, int y, const RowVectorXd &previous_result, const MatrixXd &next_matrix);

public:
    NondeterministicDecoding() = default;
    NondeterministicDecoding(char *filename, char *saved_filename, int depth, int initial_state);

    void Start() override;
};

#endif //NONDETERMINISTICDECODING_H
