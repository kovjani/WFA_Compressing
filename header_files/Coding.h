#ifndef CODING_H
#define CODING_H

#include <iostream>
#include <string>
#include <math.h>
#include <gtk/gtk.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

class Coding {
public:
    Coding() = default;
    Coding(char *filename);
    ~Coding();
    void Start();

private:
    GError *error = NULL;
    GdkPixbuf *pixbuf{};
    double *quadtree{};
    int width{};
    int height{};
    int coding_image_size{};
    int depth{};
    int quadtree_size{};
    double CreateQuadtree(int level, int index, int x, int y);
};

#endif //CODING_H
