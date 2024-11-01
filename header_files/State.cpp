#include "State.h"

int State::states_count = 0;

State::State() : Quadrant() {
    State::states_count++;
}

State::State(const Quadrant *q) : Quadrant(q->brightness) {
    this->index = State::states_count++;
    this->brightness = q->brightness;
    this->parent = q->parent;
    this->a = q->a;
    this->b = q->b;
    this->c = q->c;
    this->d = q->d;
}

State::~State() {
    State::states_count--;
}
