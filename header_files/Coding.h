#ifndef CODING_H
#define CODING_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <functional>
#include <gtk/gtk.h>
#include <glib.h>

#include "Quadrant.h"

class Coding {
protected:
    GError *error{nullptr};
    GdkPixbuf *pixbuf{};

    Quadrant *quadtree{}, *states{};

    const char *directory{}, *saved_filename{};

    int width{0};
    int height{0};
    int coding_image_size{0};
    int depth{0};
    int quadtree_size{0};
    double EPS{0.0001};
    int rounded{4};
    int states_counter{1};

    double CreateQuadtree(int level, int index, int x, int y);
    double CompareQuadrants(const Quadrant &q1, const Quadrant &q2) const;
    void GetQuadrants(int depth, int &index, const Quadrant &q, Quadrant *quadrants) const;

    virtual void CreateWFA() = 0;
    virtual void ScanState(Quadrant &quadrant, char quadrant_symbol, int &parent_state_index) = 0;
    virtual void SaveWFA(const char *filename) = 0;

public:
    Coding() = default;
    Coding(const char *opened_filename, const char *saved_filename);
    virtual ~Coding();
    void Start();
};

#endif //CODING_H