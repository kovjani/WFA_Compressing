#ifndef CODING_H
#define CODING_H

#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

#include <functional>
#include <gtk/gtk.h>
#include <glib.h>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/IterativeLinearSolvers>

#include "Quadrant.h"
#include "Transition.h"

using namespace std;
using namespace Eigen;

class NondeterministicCoding {
public:
    NondeterministicCoding() = default;
    explicit NondeterministicCoding(const char *filename, double epsilon, int number_of_states, int details, char *saved_filename);
    ~NondeterministicCoding();
    void Start();


private:
    GError *error{nullptr};
    GdkPixbuf *pixbuf{};
    char *directory{}, *saved_filename{};
    // Transition **A{}, **B{}, **C{}, **D{};
    MatrixXd A{}, B{}, C{}, D{};
    VectorXd F{};
    Quadrant *quadtree{}, *states{};
    int width{0};
    int height{0};
    int coding_image_size{0};
    int depth{0};
    double EPS{0.0001};
    int calling_counter{0};
    int quadtree_size{0};
    int states_counter{0};
    int number_of_states{0};
    int details{4};
    /*int A_counter{0};
    int B_counter{0};
    int C_counter{0};
    int D_counter{0};*/

    Quadrant* CreateQuadtree(int level, int index, int x, int y);
    //Quadrant *CreateBinarytree(int level, int index, int x, int y);
    // int CreateWFA(int i, int k, int max);
    void CreateWFA();
    VectorXd FindCoefficients(const VectorXd& phi);
    void ScanState(Quadrant &quadrant, char quadrant_symbol, int parent_state_index);
    bool CheckQuadrants(int index);
    // bool CheckQuadrants(int level, int i, const RowVectorXd &previous_result, const MatrixXd &next_matrix)
    [[nodiscard]] double CompareQuadrants(const Quadrant &q1, const Quadrant &q2) const;
    void SaveWFA(const char *filename);

    const Quadrant& operator[](unsigned i) const;
};

#endif //CODING_H