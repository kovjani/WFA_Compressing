#include "../header_files/Element.h"

Element::Element(double value) {
    this->value = value;
}

Element::Element(int j, double value) {
    this->j = j;
    this->value = value;
}

Element::Element(int i, int j, double value) {
    this->i = i;
    this->j = j;
    this->value = value;
}

// Sort elements by i.
bool Element::operator< (const Element &other) const {
    return this->i < other.i;
}
