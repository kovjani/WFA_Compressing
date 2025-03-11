#include "../header_files/NondeterministicDecoding.h"

NondeterministicDecoding::NondeterministicDecoding(char *filename, char *saved_filename, int depth, int initial_state)
    : Decoding(filename, saved_filename, depth) {

    string line, token;

    ifstream wfa_file(filename);

    // The first line is the size of the square matrices
    getline(wfa_file, line);
    this->n = stoi(line);

    getline(wfa_file, line);     // \n

    // Allocate memory
    this->I.resize(this->n);
    this->F.resize(this->n);

    this->Inn.resize(this->n, this->n);

    this->A.resize(this->n, this->n);
    this->B.resize(this->n, this->n);
    this->C.resize(this->n, this->n);
    this->D.resize(this->n, this->n);

    // Initialize I
    this->I.setIdentity();

    // Initialize the Inn, it's an nxn identity matrix for the first call.
    this->Inn.setIdentity();

    initial_state--; //the first state is the 0. index

    // Set I as an identity vector according to initial state.
    if(initial_state < I.size())
        for (int i = 0; i < I.size(); ++i) {
            I(i) = Inn(initial_state, i);
        }

    // Initialize F
    // The second line is the F (n*1 matrix), the average colors of states.
    {
        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int i = 0; i < this->n; ++i) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->F(i) = stod(token) * 255;
        }
    }

    getline(wfa_file, line);    // \n

    // Initialize matrix A
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int j = 0; j < this->n; ++j) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->A(i, j) = stod(token);
        }
    }

    getline(wfa_file, line);     // \n

    // Initialize matrix B
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int j = 0; j < this->n; ++j) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->B(i, j) = stod(token);
        }
    }

    getline(wfa_file, line);     // \n

    // Initialize matrix C
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int j = 0; j < this->n; ++j) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->C(i, j) = stod(token);
        }
    }

    getline(wfa_file, line);    // \n

    // Initialize matrix D
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int j = 0; j < this->n; ++j) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->D(i, j) = stod(token);
        }
    }
}

void NondeterministicDecoding::Start() {
    DecodePixelsColors(this->depth, 0, 0, this->I, this->Inn);
    char *saved_file_path = SaveDecodedImage(this->directory, this->saved_filename);

    OpenImage(saved_file_path);

    g_free(saved_file_path);
}

void NondeterministicDecoding::DecodePixelsColors(int level, int x, int y, const RowVectorXd &previous_result, const MatrixXd &next_matrix) {
    if(level > 0 ) {
        // Matrix multiplication
        RowVectorXd result = previous_result * next_matrix;

        int quadrant_size = 1 << level; // 2^level

        // Call recursively for all quadrants
        DecodePixelsColors(level-1, x, y, result, this->A);
        DecodePixelsColors(level-1, x + quadrant_size/2, y, result, this->B);
        DecodePixelsColors(level-1, x, y + quadrant_size/2, result, this->C);
        DecodePixelsColors(level-1, x + quadrant_size/2, y + quadrant_size/2,result, this->D);

        return;
    }
    // else
    // Pixels
    double average_color = previous_result.dot(this->F);

    this->pixels_colors[x * this->decoding_image_size + y] = average_color;
}
