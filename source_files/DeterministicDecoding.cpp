#include "../header_files/DeterministicDecoding.h"

#include "../header_files/Transition.h"

DeterministicDecoding::DeterministicDecoding(char *filename, char *saved_filename, int depth, int initial_state)
    : Decoding(filename, saved_filename, depth) {
    // Open a file in read mode

    string line, token;

    ifstream wfa_file(filename);

    // The first line is the size of the square matrices
    getline(wfa_file, line);
    this->n = stoi(line);

    getline(wfa_file, line);     // \n

    // Allocate memory
    this->Inn = new Transition[this->n];
    this->F = new Transition[this->n];

    this->A = new Transition[this->n];
    this->B = new Transition[this->n];
    this->C = new Transition[this->n];
    this->D = new Transition[this->n];

    // Initialize I
    initial_state--;
    this->I = Transition(initial_state < this->n ? initial_state : 0, 1);

    // Initialize the Inn, it's an nxn identity matrix for the first call.
    for (int i = 0; i < this->n; ++i) {
        this->Inn[i] = Transition(i, 1);
    }

    // Initialize F
    // The second line is the F (n*1 matrix), the average colors of states.
    {
        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int i = 0; i < this->n; ++i) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->F[i] = Transition(0, stod(token) * 255);
        }
    }

    getline(wfa_file, line);    // \n

    // Initialize matrix A
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        getline(line_stream, token, ' ');
        int j= stoi(token);

        // If there is no cost in a line, the cost is 1 by default.
        double value = 1;
        if(getline(line_stream, token, ' ')) {
            value = stod(token);
        }

        this->A[i] = Transition(j, value);
    }

    getline(wfa_file, line);     // \n

    // Initialize matrix B
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        getline(line_stream, token, ' ');
        int j= stoi(token);

        // If there is no cost in a line, the cost is 1 by default.
        double value = 1;
        if(getline(line_stream, token, ' ')) {
            value = stod(token);
        }

        this->B[i] = Transition(j, value);
    }

    getline(wfa_file, line);     // \n

    // Initialize matrix C
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        getline(line_stream, token, ' ');
        int j= stoi(token);

        // If there is no cost in a line, the cost is 1 by default.
        double value = 1;
        if(getline(line_stream, token, ' ')) {
            value = stod(token);
        }

        this->C[i] = Transition(j, value);
    }

    getline(wfa_file, line);    // \n

    // Initialize matrix D
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        getline(line_stream, token, ' ');
        int j= stoi(token);

        // If there is no cost in a line, the cost is 1 by default.
        double value = 1;
        if(getline(line_stream, token, ' ')) {
            value = stod(token);
        }

        this->D[i] = Transition(j, value);
    }
}

DeterministicDecoding::~DeterministicDecoding() {
    // Free the allocated memory

    delete[] this->A;
    this->A = nullptr;

    delete[] this->B;
    this->B = nullptr;

    delete[] this->C;
    this->C = nullptr;

    delete[] this->D;
    this->D = nullptr;

    delete[] this->Inn;
    this->Inn = nullptr;

    delete[] this->F;
    this->F = nullptr;
}

void DeterministicDecoding::Start() {
    DecodePixelsColors(this->depth, 0, 0, this->I, this->Inn);
    char *saved_file_path = SaveDecodedImage(this->directory, this->saved_filename);

    OpenImage(saved_file_path);

    g_free(saved_file_path);
}

void DeterministicDecoding::DecodePixelsColors(int level, int x, int y, const Transition &previous_matrix, Transition *next_matrix) {

    if(level > 0 ) {
        double value = 0;
        int res_j = 0;

        // Matrix multiplication
        // Previous_matrix always has one element, because the automata is deterministic.
        const Transition pre = previous_matrix;
        for (int i = 0; i < this->n; ++i) {
            Transition next = next_matrix[i];
            // i = next->i
            if(pre.j == i) {
                value = pre.value * next.value;
                // pre->i is always 0, because the previous matrix is a row vector
                res_j = next.j;
                break;
            }
        }

        const Transition result = Transition(res_j, value);
        int quadrant_size = 1 << level; // 2^level

        DecodePixelsColors(level-1, x, y, result, this->A);
        DecodePixelsColors(level-1, x + quadrant_size/2, y, result, this->B);
        DecodePixelsColors(level-1, x, y + quadrant_size/2, result, this->C);
        DecodePixelsColors(level-1, x + quadrant_size/2, y + quadrant_size/2,result, this->D);

        return;
    }
    // else
    // Pixels
    double average_color = 0;
    const Transition pre = previous_matrix;

    for (int i = 0; i < this->n; ++i) {
        // i = this->F[i]->i
        if(pre.j == i) {
            average_color = pre.value * this->F[i].value;
            break;
        }
    }

    this->pixels_colors[x * this->decoding_image_size + y] = average_color;
}