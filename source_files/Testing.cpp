#include "../header_files/Testing.h"

Testing::Testing(int n, int depth) {
    this->n = n;
    this->depth = depth;
    this->decoding_image_size = 1 << depth; // The size of the image (2^depth)

    // Allocate memory for the matrices
    this->I = gsl_matrix_alloc(1, this->n);
    this->F = gsl_matrix_alloc(this->n, 1);

    this->Inn = gsl_matrix_alloc(this->n, this->n);

    this->A = gsl_matrix_alloc(this->n, this->n);
    this->B = gsl_matrix_alloc(this->n, this->n);
    this->C = gsl_matrix_alloc(this->n, this->n);
    this->D = gsl_matrix_alloc(this->n, this->n);

    // Initialize I, Inn and F
    gsl_matrix_set_identity(this->Inn);
    gsl_matrix_set_identity(this->I);

    gsl_matrix_set(this->F, 0, 0, 255);
}
Testing::~Testing() {
    // Free the allocated memory
    gsl_matrix_free(this->A);
    gsl_matrix_free(this->B);
    gsl_matrix_free(this->C);
    gsl_matrix_free(this->D);

    gsl_matrix_free(this->Inn);

    gsl_matrix_free(this->I);
    gsl_matrix_free(this->F);
}

void Testing::Start() {
    switch (this->n) {
        case 1:
            Test1();
        break;
        case 2:
            Test2();
        break;
    }
}

void Testing::Test1() {

    int counter = 0;

    for (int i = 0; i < 2; i++) {
        gsl_matrix_set(this->A, 0, 0, i);
        for (int j = 0; j < 2; j++) {
            gsl_matrix_set(this->B, 0, 0, j);
            for (int k = 0; k < 2; k++) {
                gsl_matrix_set(this->C, 0, 0, k);
                for (int l = 0; l < 2; l++) {
                    gsl_matrix_set(this->D, 0, 0, l);

                    char str[16];
                    sprintf(str, "%d", counter++);

                    Decoding decode(
                        this->A,
                        this->B,
                        this->C,
                        this->D,
                        this->F,
                        this->n,
                        this->depth,
                        true
                    );
                    decode.Start("/home/jani/kepek1", str);

                    char wfa_filename[256] = "/home/jani/kepek1_wfa/";

                    strcat(wfa_filename, str);

                    strcat(wfa_filename, ".wfa");

                    FILE *wfa_file = fopen(wfa_filename, "w");

                    if (wfa_file == NULL) {
                        printf("Error opening file!\n");
                    }

                    fprintf(wfa_file, "%d\n\n", this->n);

                    gsl_matrix_fprintf(wfa_file, this->F, "%g\n");

                    gsl_matrix_fprintf(wfa_file, this->A, "%g\n");
                    gsl_matrix_fprintf(wfa_file, this->B, "%g\n");
                    gsl_matrix_fprintf(wfa_file, this->C, "%g\n");
                    gsl_matrix_fprintf(wfa_file, this->D, "%g\n");

                    fclose(wfa_file);
                }
            }
        }
    }
}

void Testing::Test2() {
    int counter = 0;

    for (int i = 0; i < 2; ++i) {
        gsl_matrix_set(this->A, 0, 0, i);
        for (int j = 0; j < 2; ++j) {
            gsl_matrix_set(this->A, 0, 1, j);
            for (int k = 0; k < 2; ++k) {
                gsl_matrix_set(this->A, 1, 0, k);
                for (int l = 0; l < 2; ++l) {
                    gsl_matrix_set(this->A, 1, 1, l);

                    for (int i2 = 0; i2 < 2; ++i2) {
                        gsl_matrix_set(this->B, 0, 0, i2);
                        for (int j2 = 0; j2 < 2; ++j2) {
                            gsl_matrix_set(this->B, 0, 1, j2);
                            for (int k2 = 0; k2 < 2; ++k2) {
                                gsl_matrix_set(this->B, 1, 0, k2);
                                for (int l2 = 0; l2 < 2; ++l2) {
                                    gsl_matrix_set(this->B, 1, 1, l2);

                                    for (int i3 = 0; i3 < 2; ++i3) {
                                        gsl_matrix_set(this->C, 0, 0, i3);
                                        for (int j3 = 0; j3 < 2; ++j3) {
                                            gsl_matrix_set(this->C, 0, 1, j3);
                                            for (int k3 = 0; k3 < 2; ++k3) {
                                                gsl_matrix_set(this->C, 1, 0, k3);
                                                for (int l3 = 0; l3 < 2; ++l3) {
                                                    gsl_matrix_set(this->C, 1, 1, l3);

                                                    for (int i4 = 0; i4 < 2; ++i4) {
                                                        gsl_matrix_set(this->D, 0, 0, i4);
                                                        for (int j4 = 0; j4 < 2; ++j4) {
                                                            gsl_matrix_set(this->D, 0, 1, j4);
                                                            for (int k4 = 0; k4 < 2; ++k4) {
                                                                gsl_matrix_set(this->D, 1, 0, k4);
                                                                for (int l4 = 0; l4 < 2; ++l4) {
                                                                    gsl_matrix_set(this->D, 1, 1, l4);

                                                                    char str[16];
                                                                    sprintf(str, "%d", counter++);

                                                                    Decoding decode(
                                                                        this->A,
                                                                        this->B,
                                                                        this->C,
                                                                        this->D,
                                                                        this->F,
                                                                        this->n,
                                                                        this->depth,
                                                                        true
                                                                    );
                                                                    decode.Start("/home/jani/kepek2", str);

                                                                    char wfa_filename[256] = "/home/jani/kepek2_wfa/";

                                                                    strcat(wfa_filename, str);
                                                                    strcat(wfa_filename, ".wfa");

                                                                    FILE *wfa_file = fopen(wfa_filename, "w");

                                                                    if (wfa_file == NULL) {
                                                                        printf("Error opening file!\n");
                                                                    }

                                                                    fprintf(wfa_file, "%d\n\n", n);

                                                                    fprintf(wfa_file, "%g %g\n\n",
                                                                        gsl_matrix_get(this->F, 0, 0), gsl_matrix_get(this->F, 1, 0));

                                                                    fprintf(wfa_file, "%g %g\n%g %g\n\n",
                                                                        gsl_matrix_get(this->A, 0, 0), gsl_matrix_get(this->A, 0, 1),
                                                                        gsl_matrix_get(this->A, 1, 0), gsl_matrix_get(this->A, 1, 1));

                                                                    fprintf(wfa_file, "%g %g\n%g %g\n\n",
                                                                        gsl_matrix_get(this->B, 0, 0), gsl_matrix_get(this->B, 0, 1),
                                                                        gsl_matrix_get(this->B, 1, 0), gsl_matrix_get(this->B, 1, 1));

                                                                    fprintf(wfa_file, "%g %g\n%g %g\n\n",
                                                                        gsl_matrix_get(this->C, 0, 0), gsl_matrix_get(this->C, 0, 1),
                                                                        gsl_matrix_get(this->C, 1, 0), gsl_matrix_get(this->C, 1, 1));

                                                                    fprintf(wfa_file, "%g %g\n%g %g\n\n",
                                                                        gsl_matrix_get(this->D, 0, 0), gsl_matrix_get(this->D, 0, 1),
                                                                        gsl_matrix_get(this->D, 1, 0), gsl_matrix_get(this->D, 1, 1));


                                                                    fclose(wfa_file);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}