#ifndef DECODING_H
#define DECODING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>
#include <gtk/gtk.h>

class Decoding {
protected:
    char *directory{}, *saved_filename{};

    int depth{}; // Decoding depth, maximum depth of recursion
    int decoding_image_size{}; // The size of the image (2^depth)
    uint8_t *image_pixels{};

    char *SaveDecodedImage(char *directory, char *filename) const;
    static void OpenImage(char *full_path) ;

public:
    Decoding() = default;
    Decoding(char *filename, char *saved_filename, int depth);
    virtual ~Decoding();

    //pure virtual method
    virtual void Start() = 0;
};


#endif //DECODING_H