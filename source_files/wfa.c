#include "../header_files/wfa.h"

gsl_matrix *A, *B, *C, *D, *I, *Inn, *F;
int n;  // The size of matrix.
int depth; // Decoding resolution, maximum depth of recursion
int decoding_image_size; // The size of the image (2^res)

void OpenImage(char *full_path) {
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *vbox;

    //gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image Loader");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);

    gtk_image_set_from_file(GTK_IMAGE(image), full_path);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();
}

void SaveDecodedImage(char *directory, double *pixels_colors, char *filename) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, decoding_image_size, decoding_image_size);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    for (int y = 0; y < decoding_image_size; ++y) {
        for (int x = 0; x < decoding_image_size; ++x) {
            guchar *p = pixels + y * rowstride + x * n_channels;
            p[0] = p[1] = p[2] = pixels_colors[y * decoding_image_size + x]; // Set R, G, B to the grayscale value
        }
    }
    char *filename_with_path = g_build_filename(directory, filename, NULL);

    gdk_pixbuf_save(pixbuf, filename_with_path, "png", NULL, NULL);

    g_print("WFA Image saved to %s\n", filename_with_path);

    OpenImage(filename_with_path);

    g_object_unref(pixbuf);
    g_free(filename_with_path);
}

double DecodePixelsColors(int level, double *pixels_colors, int x, int y, gsl_matrix *previous_matrix, gsl_matrix *next_matrix){

    gsl_matrix *result = gsl_matrix_alloc(1, n);

    // matrix multiplication
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, previous_matrix, next_matrix, 0.0, result);

    int quadrant_size = 1 << level; // 2^level

    if(level > 0) {
        double a = DecodePixelsColors(level-1, pixels_colors, x, y, result, A);
        double b = DecodePixelsColors(level-1, pixels_colors, x + (int)floor(quadrant_size/2), y, result, B);
        double c = DecodePixelsColors(level-1, pixels_colors, x, y + (int)floor(quadrant_size/2), result, C);
        double d = DecodePixelsColors(level-1, pixels_colors, x + (int)floor(quadrant_size/2), y + (int)floor(quadrant_size/2),result, D);

        gsl_matrix_free(result);

        return (a+b+c+d)/4;
    }
    // else
        // Pixels
        gsl_matrix *average_color = gsl_matrix_alloc(1, 1);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, result, F, 0.0, average_color);

        pixels_colors[y * decoding_image_size + x] = gsl_matrix_get(average_color, 0, 0);

        double avg_color = gsl_matrix_get(average_color, 0, 0);

        gsl_matrix_free(average_color);
        gsl_matrix_free(result);

        return avg_color;
}

void Test2() {

    n = 2;
    depth = 6;
    decoding_image_size = 1 << depth; // The size of the image (2^depth)

    // Allocate memory for the matrices
    I = gsl_matrix_alloc(1, n);
    F = gsl_matrix_alloc(n, 1);

    Inn = gsl_matrix_alloc(n, n);

    A = gsl_matrix_alloc(n, n);
    B = gsl_matrix_alloc(n, n);
    C = gsl_matrix_alloc(n, n);
    D = gsl_matrix_alloc(n, n);

    // Initialize I, Inn and F
    gsl_matrix_set_identity(Inn);
    gsl_matrix_set_identity(I);

    gsl_matrix_set(F, 0, 0, 0);
    gsl_matrix_set(F, 1, 0, 255);

    double *pixels_colors = (double *)malloc(decoding_image_size*decoding_image_size*sizeof(double));

    int counter = 0;

    for (int i = 0; i < 2; ++i) {
        gsl_matrix_set(A, 0, 0, i);
        for (int j = 0; j < 2; ++j) {
            gsl_matrix_set(A, 0, 1, j);
            for (int k = 0; k < 2; ++k) {
                gsl_matrix_set(A, 1, 0, k);
                for (int l = 0; l < 2; ++l) {
                    gsl_matrix_set(A, 1, 1, l);

                    for (int i2 = 0; i2 < 2; ++i2) {
                        gsl_matrix_set(B, 0, 0, i2);
                        for (int j2 = 0; j2 < 2; ++j2) {
                            gsl_matrix_set(B, 0, 1, j2);
                            for (int k2 = 0; k2 < 2; ++k2) {
                                gsl_matrix_set(B, 1, 0, k2);
                                for (int l2 = 0; l2 < 2; ++l2) {
                                    gsl_matrix_set(B, 1, 1, l2);

                                    for (int i3 = 0; i3 < 2; ++i3) {
                                        gsl_matrix_set(C, 0, 0, i3);
                                        for (int j3 = 0; j3 < 2; ++j3) {
                                            gsl_matrix_set(C, 0, 1, j3);
                                            for (int k3 = 0; k3 < 2; ++k3) {
                                                gsl_matrix_set(C, 1, 0, k3);
                                                for (int l3 = 0; l3 < 2; ++l3) {
                                                    gsl_matrix_set(C, 1, 1, l3);

                                                    for (int i4 = 0; i4 < 2; ++i4) {
                                                        gsl_matrix_set(D, 0, 0, i4);
                                                        for (int j4 = 0; j4 < 2; ++j4) {
                                                            gsl_matrix_set(D, 0, 1, j4);
                                                            for (int k4 = 0; k4 < 2; ++k4) {
                                                                gsl_matrix_set(D, 1, 0, k4);
                                                                for (int l4 = 0; l4 < 2; ++l4) {
                                                                    gsl_matrix_set(D, 1, 1, l4);

                                                                    DecodePixelsColors(depth, pixels_colors, 0, 0, I, Inn);

                                                                    char str[16];
                                                                    sprintf(str, "%d", counter++);
                                                                    SaveDecodedImage("/home/jani/kepek2", pixels_colors, str);

                                                                    char wfa_filename[256] = "/home/jani/kepek2_wfa/";

                                                                    strcat(wfa_filename, str);
                                                                    strcat(wfa_filename, ".wfa");

                                                                    FILE *wfa_file = fopen(wfa_filename, "w");

                                                                    if (wfa_file == NULL) {
                                                                        printf("Error opening file!\n");
                                                                    }

                                                                    fprintf(wfa_file, "%d\n\n", n);

                                                                    fprintf(wfa_file, "%g %g\n\n",
                                                                        gsl_matrix_get(F, 0, 0), gsl_matrix_get(F, 1, 0));

                                                                    fprintf(wfa_file, "%g %g\n%g %g\n\n",
                                                                        gsl_matrix_get(A, 0, 0), gsl_matrix_get(A, 0, 1),
                                                                        gsl_matrix_get(A, 1, 0), gsl_matrix_get(A, 1, 1));

                                                                    fprintf(wfa_file, "%g %g\n%g %g\n\n",
                                                                        gsl_matrix_get(B, 0, 0), gsl_matrix_get(B, 0, 1),
                                                                        gsl_matrix_get(B, 1, 0), gsl_matrix_get(B, 1, 1));

                                                                    fprintf(wfa_file, "%g %g\n%g %g\n\n",
                                                                        gsl_matrix_get(C, 0, 0), gsl_matrix_get(C, 0, 1),
                                                                        gsl_matrix_get(C, 1, 0), gsl_matrix_get(C, 1, 1));

                                                                    fprintf(wfa_file, "%g %g\n%g %g\n\n",
                                                                        gsl_matrix_get(D, 0, 0), gsl_matrix_get(D, 0, 1),
                                                                        gsl_matrix_get(D, 1, 0), gsl_matrix_get(D, 1, 1));


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

    // Free the allocated memory
    gsl_matrix_free(A);
    gsl_matrix_free(B);
    gsl_matrix_free(C);
    gsl_matrix_free(D);

    gsl_matrix_free(Inn);

    gsl_matrix_free(I);
    gsl_matrix_free(F);

    g_free(pixels_colors);
}

void Test1() {

    n = 1;
    depth = 6;
    decoding_image_size = 1 << depth; // The size of the image (2^depth)

    // Allocate memory for the matrices
    I = gsl_matrix_alloc(1, n);
    F = gsl_matrix_alloc(n, 1);

    Inn = gsl_matrix_alloc(n, n);

    A = gsl_matrix_alloc(n, n);
    B = gsl_matrix_alloc(n, n);
    C = gsl_matrix_alloc(n, n);
    D = gsl_matrix_alloc(n, n);

    // Initialize I, Inn and F
    gsl_matrix_set_identity(Inn);
    gsl_matrix_set_identity(I);

    gsl_matrix_set(F, 0, 0, 255);

    double *pixels_colors = (double *)malloc(decoding_image_size*decoding_image_size*sizeof(double));

    int counter = 0;

    for (int i = 0; i < 2; i++) {
        gsl_matrix_set(A, 0, 0, i);
        for (int j = 0; j < 2; j++) {
            gsl_matrix_set(B, 0, 0, j);
            for (int k = 0; k < 2; k++) {
                gsl_matrix_set(C, 0, 0, k);
                for (int l = 0; l < 2; l++) {
                    gsl_matrix_set(D, 0, 0, l);

                    DecodePixelsColors(depth, pixels_colors, 0, 0, I, Inn);

                    char str[16];
                    sprintf(str, "%d", counter++);
                    SaveDecodedImage("/home/jani/kepek1", pixels_colors, str);

                    char wfa_filename[256] = "/home/jani/kepek1_wfa/";

                    strcat(wfa_filename, str);

                    strcat(wfa_filename, ".wfa");

                    FILE *wfa_file = fopen(wfa_filename, "w");

                    if (wfa_file == NULL) {
                        printf("Error opening file!\n");
                    }

                    fprintf(wfa_file, "%d\n\n", n);

                    gsl_matrix_fprintf(wfa_file, F, "%g\n");

                    gsl_matrix_fprintf(wfa_file, A, "%g\n");
                    gsl_matrix_fprintf(wfa_file, B, "%g\n");
                    gsl_matrix_fprintf(wfa_file, C, "%g\n");
                    gsl_matrix_fprintf(wfa_file, D, "%g\n");

                    fclose(wfa_file);
                }
            }
        }
    }

    // Free the allocated memory
    gsl_matrix_free(A);
    gsl_matrix_free(B);
    gsl_matrix_free(C);
    gsl_matrix_free(D);

    gsl_matrix_free(Inn);

    gsl_matrix_free(I);
    gsl_matrix_free(F);

    g_free(pixels_colors);
}

void GrayWFADecode(GtkWidget *clicked_button, gpointer file_chooser_button) {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    if (filename) {
        char *directory = g_path_get_dirname(filename);
        FILE *wfa_file;

        // Open a file in read mode
        wfa_file = fopen(filename, "r");

        // Check if the file was opened successfully
        if (wfa_file == NULL) {
            g_print("Failed to open the file.\n");
            return;
        }

        char line[256];

        // The first line is the size of the square matrices
        fgets(line, sizeof(line), wfa_file);
        n = atoi(line);

        fgets(line, sizeof(line), wfa_file);     // \n

        // Allocate memory for the matrices
        I = gsl_matrix_alloc(1, n);
        F = gsl_matrix_alloc(n, 1);

        Inn = gsl_matrix_alloc(n, n);

        A = gsl_matrix_alloc(n, n);
        B = gsl_matrix_alloc(n, n);
        C = gsl_matrix_alloc(n, n);
        D = gsl_matrix_alloc(n, n);

        // Initialize I
        gsl_matrix_set_identity(I);

        // Initialize the Inn, it's an nxn identity matrix for the first call.
        gsl_matrix_set_identity(Inn);

        // Initialize F
        // The second line is the F (n*1 matrix), the average colors of states.
        {
            fgets(line, sizeof(line), wfa_file);
            char *token = strtok(line, " "); // Split by space

            for (int i = 0; i < n; ++i) {
                gsl_matrix_set(F, i, 0, atof(token));
                token = strtok(NULL, " "); // Get the next token
            }

            g_free(token);
        }

        fgets(line, sizeof(line), wfa_file);     // \n

        // Initialize matrix A
        for (int i = 0; i < n; ++i) {

            fgets(line, sizeof(line), wfa_file);
            char *token = strtok(line, " "); // Split by space

            for (int j = 0; j < n; ++j) {
                gsl_matrix_set(A, i, j, atof(token));
                token = strtok(NULL, " "); // Get the next token
            }

            g_free(token);
        }

        fgets(line, sizeof(line), wfa_file);     // \n

        // Initialize matrix B
        for (int i = 0; i < n; ++i) {

            fgets(line, sizeof(line), wfa_file);
            char *token = strtok(line, " "); // Split by space

            for (int j = 0; j < n; ++j) {
                gsl_matrix_set(B, i, j, atof(token));
                token = strtok(NULL, " "); // Get the next token
            }

            g_free(token);
        }

        fgets(line, sizeof(line), wfa_file);     // \n

        // Initialize matrix C
        for (int i = 0; i < n; ++i) {

            fgets(line, sizeof(line), wfa_file);
            char *token = strtok(line, " "); // Split by space

            for (int j = 0; j < n; ++j) {
                gsl_matrix_set(C, i, j, atof(token));
                token = strtok(NULL, " "); // Get the next token
            }

            g_free(token);
        }

        fgets(line, sizeof(line), wfa_file);     // \n

        // Initialize matrix D
        for (int i = 0; i < n; ++i) {

            fgets(line, sizeof(line), wfa_file);
            char *token = strtok(line, " "); // Split by space

            for (int j = 0; j < n; ++j) {
                gsl_matrix_set(D, i, j, atof(token));
                token = strtok(NULL, " "); // Get the next token
            }

            g_free(token);
        }

        depth = 9;
        decoding_image_size = 1 << depth; // The size of the image (2^res)

        double *pixels_colors = (double *)malloc(decoding_image_size*decoding_image_size*sizeof(double));

        DecodePixelsColors(depth, pixels_colors, 0, 0, I, Inn);

        SaveDecodedImage(directory, pixels_colors, "wfa_image.png");

        // Free the allocated memory
        gsl_matrix_free(A);
        gsl_matrix_free(B);
        gsl_matrix_free(C);
        gsl_matrix_free(D);

        gsl_matrix_free(Inn);

        gsl_matrix_free(I);
        gsl_matrix_free(F);

        g_free(filename);
        g_free(directory);
        g_free(pixels_colors);

        // Close the file
        fclose(wfa_file);
    }
}