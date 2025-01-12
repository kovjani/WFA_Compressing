#ifndef NONDETERMINISTICDECODING_H
#define NONDETERMINISTICDECODING_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <gtk/gtk.h>
#include <eigen3/Eigen/Dense>

using namespace std;
using namespace Eigen;

class NondeterministicDecoding {
public:
    NondeterministicDecoding() = default;
    NondeterministicDecoding(char *filename, int depth, double intensity);
    ~NondeterministicDecoding();
    void Start(char *directory, char *saved_filename);

private:
    //Inn is an nxn identity matrix for the first call.
    MatrixXd A{}, B{}, C{}, D{}, Inn{};
    RowVectorXd I{};
    VectorXd F{};

    int n{};  // Number of states.
    int depth{}; // Decoding resolution, maximum depth of recursion
    int decoding_image_size{}; // The size of the image (2^res)
    int calling_counter{0};
    double *pixels_colors{};
    char *SaveDecodedImage(char *directory, char *filename) const;
    void OpenImage(char *full_path) const;
    void DecodePixelsColors(int level, int x, int y, const RowVectorXd &previous_result, const MatrixXd &next_matrix);
};

#endif //NONDETERMINISTICDECODING_H
