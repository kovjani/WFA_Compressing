#ifndef DECODING_H
#define DECODING_H

#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <gtk/gtk.h>

#include "Element.h"

using namespace std;

class Decoding {
public:
    Decoding() = default;
    Decoding(char *filename, int depth, double intensity);
    ~Decoding();
    void Start(char *directory, char *saved_filename);

private:
    Element **A{}, **B{}, **C{}, **D{}, **F{}, **Inn{}; //Inn is an nxn identity matrix for the first call.
    const Element *I{};

    int n{};  // Number of states.
    int depth{}; // Decoding resolution, maximum depth of recursion
    int decoding_image_size{}; // The size of the image (2^res)
    bool testing{false};
    int calling_counter{0};
    double *pixels_colors{};
    char *SaveDecodedImage(char *directory, char *filename) const;
    void OpenImage(char *full_path) const;
    void DecodePixelsColors(int level, int x, int y, const Element* &previous_matrix, Element **next_matrix);
    void free_array_of_elements(Element** &arr, int size);
};

#endif //DECODING_H
