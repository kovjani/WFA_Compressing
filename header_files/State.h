#ifndef STATE_H
#define STATE_H
#include "Quadrant.h"

class State : public Quadrant {
public:
    static int states_count;
    State();
    explicit State(const Quadrant * q);
    ~State();
};

#endif //STATE_H