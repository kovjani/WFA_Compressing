#include "../header_files/grayquadtree.h"

/*
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

void DecodePixelsColors(int level, double *pixels_colors, int x, int y, gsl_matrix *previous_matrix, gsl_matrix *next_matrix){

    gsl_matrix *result = gsl_matrix_alloc(1, n);

    // matrix multiplication
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, previous_matrix, next_matrix, 0.0, result);

    int quadrant_size = 1 << level; // 2^level

    if(level > 0) {
        DecodePixelsColors(level-1, pixels_colors, x, y, result, A);
        DecodePixelsColors(level-1, pixels_colors, x + (int)floor(quadrant_size/2), y, result, B);
        DecodePixelsColors(level-1, pixels_colors, x, y + (int)floor(quadrant_size/2), result, C);
        DecodePixelsColors(level-1, pixels_colors, x + (int)floor(quadrant_size/2), y + (int)floor(quadrant_size/2),result, D);
    } else {
        // Pixels
        gsl_matrix *average_color = gsl_matrix_alloc(1, 1);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, result, F, 0.0, average_color);

        pixels_colors[y * size + x] = gsl_matrix_get(average_color, 0, 0);

        g_free(average_color);
    }

    g_free(result);
}
*/

int coding_image_size;
double CreateQuadtree(GdkPixbuf *pixbuf, double *quadtree, int level, int index, int x, int y){
    // Count the average color of the image segment (quadrant) according to parameters.

    if(level > 0) {
        int quadrant_size = 1 << level; // 2^level
        double A, B, C, D; // The average colors of quadrants
        A = CreateQuadtree(pixbuf, quadtree, level-1, 4 * index + 1, x, y);
        B = CreateQuadtree(pixbuf, quadtree, level-1, 4 * index + 2, x + (int)floor(quadrant_size/2), y);
        C = CreateQuadtree(pixbuf, quadtree, level-1, 4 * index + 3, x, y + (int)floor(quadrant_size/2));
        D = CreateQuadtree(pixbuf, quadtree, level-1, 4 * index + 4, x + (int)floor(quadrant_size/2), y + (int)floor(quadrant_size/2));

        // Store quadrants in a quadtree
        // When level == 1, in the (n-1). recursion level a quadrant is sum of 4 pixels.

        double quadrant_average_color = (A + B + C + D) / 4;

        // g_print("%f\n", quadrant_average_color);

       /* int index = 0;

        // The first index of this level +
        for (int i = 0; i < level; ++i) {
            index += pow(4, i);
        }

        // +
        index +=*/
        quadtree[index] = quadrant_average_color;

        return quadrant_average_color;
    }

    // else if level == 0, the n. recursion level, there are pixels.
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    guchar *p = pixels + x * rowstride + y * n_channels;
    guchar r = p[0];
    guchar g = p[1];
    guchar b = p[2];

    return (r + g + b) / 3; // Average color of the pixel
}

void WFACode(GtkWidget *clicked_button, gpointer file_chooser_button) {
    // Get the selected file name.
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    if (filename) {
        // Process the file.
        g_print("Selected file: %s\n", filename);


        //Open the image and split it into pieces.

        // Open image
        GError *error = NULL;
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, &error);
        if (!pixbuf) {
            g_print(stderr, "Error loading image: %s\n", error->message);
            g_error_free(error);
            return;
        }

        // Compare pixel colors and create automata.

        int width = gdk_pixbuf_get_width(pixbuf);
        int height = gdk_pixbuf_get_height(pixbuf);
        coding_image_size = width > height ? height : width;

        /*AverageCounter(pixbuf, &full_image);

        g_print("%d, %d, %d, %f, %f, %f\n", full_image.x, full_image.y, full_image.size, full_image.color[0], full_image.color[1], full_image.color[2]);*/

        // A full quadtree for a (2^level*2^level) image has ((4^level - 1) / 3) nodes.
        // The number of leaves in a full quadtree is (4^level).
        // We don't want to store the leaves.
        int level = (int)log2(coding_image_size); // maximum recursion level
        int quadtree_size = 0;
        for (int i = 0; i < level; ++i) {
            quadtree_size += pow(4, i);
        }
        double *quadtree = (double *)malloc( quadtree_size * sizeof(double) );

        for (int i = 0; i < quadtree_size; i++) {
            quadtree[i] = -1;
        }

        CreateQuadtree(pixbuf, quadtree, level, 0, 0, 0);

        for (int i = 0; i < quadtree_size; i++) {
            g_print("%f ", quadtree[i]);
        }


        // Free the allocated memory.
        g_object_unref(pixbuf);
        g_free(filename);
        g_free(quadtree);
    }
}