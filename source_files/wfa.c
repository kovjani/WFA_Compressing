#include "../header_files/wfa.h"

gsl_matrix *A, *B, *C, *D, *I, *F;
int n;  // The size of matrix.
int res; // Decoding resolution
int size; // The size of the image (2^res)

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

void SaveDecodedImage(char *directory, double *pixels_colors) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, size, size);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);;
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            guchar *p = pixels + y * rowstride + x * n_channels;
            p[0] = p[1] = p[2] = pixels_colors[y * size + x]; // Set R, G, B to the grayscale value
        }
    }
    char *full_path = g_build_filename(directory, "wfa_image.png", NULL);

    gdk_pixbuf_save(pixbuf, full_path, "png", NULL, NULL);

    g_print("WFA Image saved to %s\n", full_path);

    OpenImage(full_path);

    g_object_unref(pixbuf);
    g_free(full_path);
}

void DecodeQuadTree(int level, double *pixels_colors, int x, int y, gsl_matrix *previous_matrix, gsl_matrix *next_matrix){

    gsl_matrix *result = gsl_matrix_alloc(1, n);

    // matrix multiplication
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, previous_matrix, next_matrix, 0.0, result);

    int quadrant_size = 1 << level; // 2^level

    if(level > 0) {
        DecodeQuadTree(level-1, pixels_colors, x, y, result, A);
        DecodeQuadTree(level-1, pixels_colors, x + (int)floor(quadrant_size/2), y, result, B);
        DecodeQuadTree(level-1, pixels_colors, x, y + (int)floor(quadrant_size/2), result, C);
        DecodeQuadTree(level-1, pixels_colors, x + (int)floor(quadrant_size/2), y + (int)floor(quadrant_size/2),result, D);
    } else {
        // Pixels
        gsl_matrix *average_color = gsl_matrix_alloc(1, 1);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, result, F, 0.0, average_color);

        pixels_colors[y * size + x] = gsl_matrix_get(average_color, 0, 0);

        g_free(average_color);
    }

    g_free(result);
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

        A = gsl_matrix_alloc(n, n);
        B = gsl_matrix_alloc(n, n);
        C = gsl_matrix_alloc(n, n);
        D = gsl_matrix_alloc(n, n);

        // Initialize I
        gsl_matrix_set_identity(I);

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

        res = 9;
        size = 1 << res; // The size of the image (2^res)

        double pixels_colors[size*size];

        // DecodeQuadTree() with res is the epsilon call which calls 4 times this function with res-1 and the other parameters

        DecodeQuadTree(res-1, pixels_colors, 0, 0, I, A);
        DecodeQuadTree(res-1, pixels_colors, (int)floor(size/2), 0, I, B);
        DecodeQuadTree(res-1, pixels_colors, 0, (int)floor(size/2), I, C);
        DecodeQuadTree(res-1, pixels_colors, (int)floor(size/2), (int)floor(size/2), I, D);

        SaveDecodedImage(directory, pixels_colors);

        // Free the allocated memory
        gsl_matrix_free(A);
        gsl_matrix_free(B);
        gsl_matrix_free(C);
        gsl_matrix_free(D);

        gsl_matrix_free(I);
        gsl_matrix_free(F);

        // Close the file
        fclose(wfa_file);

        g_free(filename);
        g_free(directory);
    }
}