#ifndef QUADRANT_H
#define QUADRANT_H

class Quadrant {
public:
    double brightness{0};
    int index{0};

    // Don't need free these pointers, because in this claas there is no memory allocation.
    Quadrant *parent;
    //Children
    Quadrant *a{}, *b{}, *c{}, *d{};

    Quadrant() = default;
    explicit Quadrant(double brightness);
    ~Quadrant() = default;

   /* Quadrant& operator=(const Quadrant& other) {
        if(this == &other) {
            return *this;
        }

        this->brightness = other.brightness;
        this->parent = other.parent;
        this->a = other.a;
        this->b = other.b;
        this->c = other.c;
        this->d = other.d;

        return *this;
    }*/
};

#endif //QUADRANT_H