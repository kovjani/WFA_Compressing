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