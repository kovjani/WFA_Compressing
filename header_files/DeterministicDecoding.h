#ifndef DETERMINISTICDECODING_H
#define DETERMINISTICDECODING_H

#include "Decoding.h"
#include "Transition.h"

using namespace std;

class DeterministicDecoding : public Decoding {
private:
    Transition *A{}, *B{}, *C{}, *D{}, *F{}, *Inn{}; //Inn is an nxn identity matrix for the first call.
    Transition I{};

    int n{};  // Number of states.

    // Same name but different params                                                              here
    void DecodePixelsColors(int level, int x, int y, const Transition &previous_matrix, Transition *next_matrix);

public:
    DeterministicDecoding() = default;
    DeterministicDecoding(char *filename, char *saved_filename, int depth, int initial_state);
    ~DeterministicDecoding() override;

    void Start() override;
};

#endif //DETERMINISTICDECODING_H