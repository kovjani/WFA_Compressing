#ifndef CODING_H
#define CODING_H

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <thread>
#include <gtk/gtk.h>
#include <glib.h>

#include "Quadrant.h"
#include "Element.h"

using namespace std;

class Coding {
public:
    Coding() = default;
    explicit Coding(const char *filename, double epsilon);
    ~Coding();
    void Start();

private:
    GError *error{nullptr};
    GdkPixbuf *pixbuf{};
    Element **A{}, **B{}, **C{}, **D{};
    Quadrant **quadtree{}, **states{};
    int width{0};
    int height{0};
    int coding_image_size{0};
    int depth{0};
    int quadtree_size{0}; // Quadtree size
    double EPS{0.0001};
    int calling_counter{0};
    int states_counter{0};

    Quadrant *CreateQuadtree(int level, int index, int x, int y);
    void CreateWFA();
    void ScanState(Quadrant &quadrant, char quadrant_symbol, int state_index, int *quadrant_index);
    [[nodiscard]] double CompareQuadrants(const Quadrant &q1, const Quadrant &q2) const;
    void SaveWFA(const char *filename);
};

#endif //CODING_H
