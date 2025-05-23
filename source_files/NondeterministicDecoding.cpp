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
    this->I = RowVectorXd(this->n);
    this->F = VectorXd(this->n);

    this->Inn = MatrixXd(this->n, this->n);

    this->A = MatrixXd(this->n, this->n);
    this->B = MatrixXd(this->n, this->n);
    this->C = MatrixXd(this->n, this->n);
    this->D = MatrixXd(this->n, this->n);

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
            this->F(i) = stod(token) * 256;
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

    // Matrix multiplication
    RowVectorXd result = previous_result * next_matrix;

    if(level > 0 ) {
        int quadrant_size = 1 << level; // 2^level

        // Call recursively for all quadrants
        DecodePixelsColors(level-1, x, y, result, this->A);
        DecodePixelsColors(level-1, x + quadrant_size/2, y, result, this->B);
        DecodePixelsColors(level-1, x, y + quadrant_size/2, result, this->C);
        DecodePixelsColors(level-1, x + quadrant_size/2, y + quadrant_size/2,result, this->D);

    } else {
        // a pixel
        double average_color = result.dot(this->F);

        this->image_pixels[x * this->decoding_image_size + y] = static_cast<uint8_t>(average_color);
    }
}