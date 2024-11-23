#ifndef ELEMENT_H
#define ELEMENT_H

class Element {
public:
    int i{0};
    int j{0};
    double value{0};

    Element() = default;
    Element(double value);
    Element(int j, double value);
    Element(int i, int j, double value);
    ~Element() = default;

    // It helps sort array of elements.
    bool operator< (const Element &other) const;
};

#endif //ELEMENT_H