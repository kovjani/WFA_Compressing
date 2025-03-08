#ifndef DETERMINISTICCODING_H
#define DETERMINISTICCODING_H

#include "Coding.h"
#include "Transition.h"

using namespace std;

class DeterministicCoding : public Coding {
private:
    Transition *A{}, *B{}, *C{}, *D{};

protected:
    void CreateWFA() override;
    void ScanState(Quadrant &quadrant, char quadrant_symbol, int &parent_state_index) override;
    void SaveWFA(const char *filename) override;

public:
    DeterministicCoding() = default;
    DeterministicCoding(const char *opened_filename, const char *saved_filename, int details, double epsilon);
    ~DeterministicCoding() override;
};

#endif //DETERMINISTICCODING_H
