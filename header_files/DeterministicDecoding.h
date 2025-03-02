#ifndef DETERMINISTICDECODING_H
#define DETERMINISTICDECODING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <gtk/gtk.h>

#include "Transition.h"

using namespace std;

class DeterministicDecoding {
public:
    DeterministicDecoding() = default;
    DeterministicDecoding(char *filename, char *saved_filename, int depth, int initial_state);
    ~DeterministicDecoding();
    void Start();

private:
    Transition **A{}, **B{}, **C{}, **D{}, **F{}, **Inn{}; //Inn is an nxn identity matrix for the first call.
    const Transition *I{};

    char *directory{}, *saved_filename{};

    int n{};  // Number of states.
    int depth{}; // Decoding resolution, maximum depth of recursion
    int decoding_image_size{}; // The size of the image (2^res)
    int calling_counter{0};
    double *pixels_colors{};
    char *SaveDecodedImage(char *directory, char *filename) const;
    void OpenImage(char *full_path) const;
    void DecodePixelsColors(int level, int x, int y, const Transition* &previous_matrix, Transition **next_matrix);
    void free_array_of_elements(Transition** &arr, int size);
};

#endif //DETERMINISTICDECODING_H
