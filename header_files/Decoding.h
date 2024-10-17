#ifndef DECODING_H
#define DECODING_H

#include <iostream>
#include <string>
#include <gtk/gtk.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

class Decoding {
public:
    Decoding() = default;
    Decoding(char *filename, int depth);
    Decoding(gsl_matrix *a, gsl_matrix *b, gsl_matrix *c, gsl_matrix *d, gsl_matrix *f, int n, int depth, bool testing);
    ~Decoding();
    /* [[nodiscard]] int getN() const;
     [[nodiscard]] int getDepth() const;
     [[nodiscard]] gsl_matrix *getA() const;
     [[nodiscard]] gsl_matrix *getB() const;
     [[nodiscard]] gsl_matrix *getC() const;
     [[nodiscard]] gsl_matrix *getD() const;
     [[nodiscard]] gsl_matrix *getF() const;*/
    void Start(char *directory, char *saved_filename);

    /*void setA(gsl_matrix *m);
    void setB(gsl_matrix *m);
    void setC(gsl_matrix *m);
    void setD(gsl_matrix *m);
    void setF(gsl_matrix *m);
    void setI(gsl_matrix *m);
    void setInn(gsl_matrix *m);
    void setN(int n);
    void setDepth(int depth);
    void setDecoding_image_size();
    void setTesting();
    void setPixels_colors();*/

private:
    gsl_matrix *A{}, *B{}, *C{}, *D{}, *F{}, *I{}, *Inn{}; //Inn is an nxn identity matrix for the first call.
    int n{};  // The size of matrix.
    int depth{}; // Decoding resolution, maximum depth of recursion
    int decoding_image_size{}; // The size of the image (2^res)
    bool testing{false};
    double *pixels_colors{};
    char *SaveDecodedImage(char *directory, char *filename) const;
    void OpenImage(char *full_path) const;
    double DecodePixelsColors(int level, int x, int y, gsl_matrix *previous_matrix, gsl_matrix *next_matrix);
};

#endif //DECODING_H
