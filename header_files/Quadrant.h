#ifndef QUADRANT_H
#define QUADRANT_H

#include <cmath>

class Quadrant {
public:
    double brightness{0};
    int state_index{0};
    int quadtree_index{0};
    int level{0};

    Quadrant() = default;
    explicit Quadrant(int level, int quadtree_index, double brightness);
    ~Quadrant() = default;
};

#endif //QUADRANT_H