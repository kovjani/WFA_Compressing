#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <string>
#include <gtk/gtk.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include "Decoding.h"

class Testing{
public:
    Testing() = default;
    Testing(int n, int depth);
    ~Testing();
    void Start();

private:
   gsl_matrix *A{}, *B{}, *C{}, *D{}, *I{}, *Inn{}, *F{}; //Inn is an nxn identity matrix for the first call.
    int n{};  // The size of matrix.
    int depth{}; // Decoding resolution, maximum depth of recursion
    int decoding_image_size{}; // The size of the image (2^res)
    void Test1();
    void Test2();
};

#endif //TEST_H