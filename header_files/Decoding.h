#ifndef DECODING_H
#define DECODING_H

#include <iostream>
#include <string>
#include <vector>
#include <gtk/gtk.h>

#include "Element.h"

class Decoding {
public:
    Decoding() = default;
    Decoding(char *filename, int depth);
    ~Decoding();
    void Start(char *directory, char *saved_filename);

private:
    Element **A{}, **B{}, **C{}, **D{}, **F{}, **Inn{}; //Inn is an nxn identity matrix for the first call.
    const Element *I{};

    int n{};  // Number of states.
    int depth{}; // Decoding resolution, maximum depth of recursion
    int decoding_image_size{}; // The size of the image (2^res)
    bool testing{false};
    double *pixels_colors{};
    char *SaveDecodedImage(char *directory, char *filename) const;
    void OpenImage(char *full_path) const;
    void DecodePixelsColors(int level, int x, int y, const Element* &previous_matrix, Element **next_matrix);
    void free_array_of_elements(Element** &arr, int size);
    void free_vector_of_elements(std::vector<Element*> &vec);
};

#endif //DECODING_H
