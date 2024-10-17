#include "../header_files/Coding.h"

Coding::Coding(char *filename) {
    //Open the image and split it into pieces.

    // Open image
    this->pixbuf = gdk_pixbuf_new_from_file(filename, &this->error);
    /*if (!pixbuf) {
        g_print(stderr, "Error loading image: %s\n", error->message);
        g_error_free(error);
        return;
    }*/

    // Compare pixel colors and create automata.

    this->width = gdk_pixbuf_get_width(this->pixbuf);
    this->height = gdk_pixbuf_get_height(this->pixbuf);
    this->coding_image_size = this->width > this->height ? this->height : this->width;

    /*AverageCounter(pixbuf, &full_image);

    g_print("%d, %d, %d, %f, %f, %f\n", full_image.x, full_image.y, full_image.size, full_image.color[0], full_image.color[1], full_image.color[2]);*/

    // A full quadtree for a (2^level*2^level) image has ((4^level - 1) / 3) nodes.
    // The number of leaves in a full quadtree is (4^level).
    // We don't want to store the leaves.
    this->depth = (int)log2(this->coding_image_size); // maximum recursion level
    this->quadtree_size = 0;
    for (int i = 0; i < this->depth; ++i) {
        this->quadtree_size += pow(4, i);
    }
    this->quadtree = (double *)malloc( this->quadtree_size * sizeof(double) );

    for (int i = 0; i < this->quadtree_size; i++) {
        this->quadtree[i] = -1;
    }
}

Coding::~Coding() {
    // Free the allocated memory.
    g_object_unref(this->pixbuf);
    g_free(this->quadtree);
}

void Coding::Start() {
    CreateQuadtree(this->depth, 0, 0, 0);

    for (int i = 0; i < this->quadtree_size; i++) {
        g_print("%f ", this->quadtree[i]);
    }
}


double Coding::CreateQuadtree(int level, int index, int x, int y){
    // Count the average color of the image segment (quadrant) according to parameters.

    if(level > 0) {
        int quadrant_size = 1 << level; // 2^level
        double A, B, C, D; // The average colors of quadrants
        A = CreateQuadtree(level-1, 4 * index + 1, x, y);
        B = CreateQuadtree(level-1, 4 * index + 2, x + (int)(quadrant_size/2), y);
        C = CreateQuadtree(level-1, 4 * index + 3, x, y + (int)(quadrant_size/2));
        D = CreateQuadtree(level-1, 4 * index + 4, x + (int)(quadrant_size/2), y + (int)(quadrant_size/2));

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
        this->quadtree[index] = quadrant_average_color;

        return quadrant_average_color;
    }

    // else if level == 0, the n. recursion level, there are pixels.
    int rowstride = gdk_pixbuf_get_rowstride(this->pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(this->pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(this->pixbuf);

    guchar *p = pixels + x * rowstride + y * n_channels;
    guchar r = p[0];
    guchar g = p[1];
    guchar b = p[2];

    return (r + g + b) / 3; // Average color of the pixel
}