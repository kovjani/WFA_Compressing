#ifndef TRANSITION_H
#define TRANSITION_H

class Transition {
public:
    int i{0};
    int j{0};
    double value{0};

    Transition() = default;
    explicit Transition(double value);
    Transition(int j, double value);
    Transition(int i, int j, double value);
    ~Transition() = default;

    // It helps sort array of elements.
    //bool operator< (const Transition &other) const;
};

#endif //TRANSITION_H