#include "../header_files/Transition.h"

Transition::Transition(double value) {
    this->value = value;
}

Transition::Transition(int j, double value) {
    this->j = j;
    this->value = value;
}

Transition::Transition(int i, int j, double value) {
    this->i = i;
    this->j = j;
    this->value = value;
}

// Sort elements by i.
/*bool Transition::operator< (const Transition &other) const {
    return this->i < other.i;
}*/
