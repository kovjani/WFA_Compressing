#ifndef DETERMINISTICCODING_H
#define DETERMINISTICCODING_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <functional>
#include <gtk/gtk.h>
#include <glib.h>

#include "Quadrant.h"
#include "Transition.h"

using namespace std;

class DeterministicCoding {
public:
    DeterministicCoding() = default;
    explicit DeterministicCoding(char *opened_filename, char *saved_filename, double epsilon);
    ~DeterministicCoding();
    void Start();

private:
    GError *error{nullptr};
    GdkPixbuf *pixbuf{};
    Transition **A{}, **B{}, **C{}, **D{};
    Quadrant **quadtree{}, **states{}, **sorted_states{};

    char *directory{}, *saved_filename{};

    int width{0};
    int height{0};
    int coding_image_size{0};
    int depth{0};
    int quadtree_size{0}; // Quadtree size
    double EPS{0.0001};
    int rounded{4};
    int states_counter{0};
    int calling_counter{0};

    Quadrant *CreateQuadtree(int level, int index, int x, int y);
    void CreateWFA();
    void ScanState(Quadrant &quadrant, char quadrant_symbol);
    [[nodiscard]] double CompareQuadrants(const Quadrant &q1, const Quadrant &q2) const;
    void SaveWFA(const char *filename);
};

#endif //DETERMINISTICCODING_H
