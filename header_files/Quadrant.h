#ifndef QUADRANT_H
#define QUADRANT_H

#include <cmath>

class Quadrant {
public:
    double brightness{0};
    int index{0};
    int level{0};

    // Don't need free these pointers, because in this claas there is no memory allocation.
    Quadrant *parent{};
    //Children
    Quadrant *a{}, *b{}, *c{}, *d{};

    Quadrant() = default;
    explicit Quadrant(double brightness);
    ~Quadrant() = default;
};

#endif //QUADRANT_H