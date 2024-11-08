#ifndef CODING_H
#define CODING_H

#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <gtk/gtk.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include "Quadrant.h"
#include "State.h"

class Coding {
public:
    Coding() = default;
    explicit Coding(const char *filename);
    ~Coding();
    void Start();
    double round_to_decimal(double value, int decimal_places);

private:
    GError *error = nullptr;
    GdkPixbuf *pixbuf{};
    gsl_matrix *A{}, *B{}, *C{}, *D{}, *F{};
    Quadrant **quadtree{};
    State **states{};
    int width{};
    int height{};
    int coding_image_size{};
    int depth{};
    int quadtree_size_1{}; // Quadtree depth - 1 size
    int quadtree_size_2{}; // Quadtree depth - 2 size
    int quadtree_size_3{}; // Quadtree depth - 3 size
    double EPS{0.1};
    double CreateQuadtree(int level, int index, int x, int y);
    void ChildPointers();
    void CreateWFA(double epsilon);
    void ScanState(Quadrant *quadrant, char quadrant_symbol, int state_index);
    double CompareQuadrants(int level, Quadrant *q1, Quadrant *q2);
    void SaveWFA(const char *filename);
};

#endif //CODING_H
