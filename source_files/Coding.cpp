#include "../header_files/Coding.h"

Coding::Coding(const char *opened_filename, const char *saved_filename, int details, double epsilon) {
    this->directory = g_path_get_dirname(opened_filename);
    this->saved_filename = saved_filename;
    this->EPS = epsilon;
    this->details = details;

    //Open the image and split it into pieces.

    this->pixbuf = gdk_pixbuf_new_from_file(opened_filename, &this->error);
    if (!pixbuf) {
        g_printerr("Error loading image: %s\n", this->error->message);
        g_error_free(error);
        return;
    }

    this->width = gdk_pixbuf_get_width(this->pixbuf);
    this->height = gdk_pixbuf_get_height(this->pixbuf);
    this->coding_image_size = this->width > this->height ? this->height : this->width;

    // A full quadtree for a (2^level*2^level) image has ((4^level - 1) / 3) nodes.
    // The number of leaves in a full quadtree is (4^level).
    // We don't want to store the leaves.
    this->depth = static_cast <int> (log2(this->coding_image_size)); // maximum recursion level
    this->quadtree_size = 0;

    // In this iteration the maximum value of i is depth-1
    for (int i = 0; i < this->depth; ++i) {
        this->quadtree_size += pow(4, i);
    }

    this->quadtree = new Quadrant[this->quadtree_size];
    // The maximum number of states is the number of quadtree nodes.
    this->states = new Quadrant [this->quadtree_size];
}

Coding::~Coding() {
    // Free the allocated memory.
    g_object_unref(this->pixbuf);

    delete[] this->quadtree;
    this->quadtree = nullptr;

    delete[] this->states;
    this->states = nullptr;
}

const Quadrant& Coding::operator[](unsigned i) const {
    double j = i;
    while(j >= this->quadtree_size) {
        j /= 4;
    }
    i = static_cast<int>(floor(j));
    return this->quadtree[i];
}

void Coding::Start() {

    CreateQuadtree(this->depth, 0, 0, 0);

    CreateWFA();

    char *filename_with_path = g_build_filename(this->directory, this->saved_filename, nullptr);

    SaveWFA(filename_with_path);

    g_free(filename_with_path);

}

double Coding::CompareQuadrants(const Quadrant &q1, const Quadrant &q2) const {

    // q1: quadrant
    // q2: state image

    // If costs are equal for all nodes according to epsilon, return cost else return -1

    // i = 1
    double q1_brightness = (*this)[this->details*q1.quadtree_index + 1].brightness;
    double q2_brightness = (*this)[this->details*q2.quadtree_index + 1].brightness;
    double cost = q1_brightness / q2_brightness;

    for (int i = 2; i <= this->details; ++i) {
        q1_brightness = (*this)[this->details*q1.quadtree_index + i].brightness;
        q2_brightness = (*this)[this->details*q2.quadtree_index + i].brightness;
        double next_cost = q1_brightness / q2_brightness;

        // if not equals
        if(fabs(cost - next_cost) > this->EPS) {
            return -1;
        }
    }

    return cost;
}

double Coding::CreateQuadtree(int level, int index, int x, int y){
    // Count the average color of the image segment (quadrant) according to parameters.

    if(level > 0) {

        const int quadrant_size = 1 << level; // 2^level
        // The average colors of quadrants
        const double a = CreateQuadtree(level-1, 4*index + 1, x, y);
        const double b = CreateQuadtree(level-1, 4*index + 2, x + quadrant_size/2, y);
        const double c = CreateQuadtree(level-1, 4*index + 3, x, y + quadrant_size/2);
        const double d = CreateQuadtree(level-1, 4*index + 4, x + quadrant_size/2, y + quadrant_size/2);

        // Store quadrants in a quadtree
        // When level == 1, in the (n-1). recursion level a quadrant is sum of 4 pixels.

        const double quadrant_average_color = (a+b+c+d) / 4;

        this->quadtree[index] = Quadrant(level, index, quadrant_average_color);

        return quadrant_average_color;
    }

    // else if level == 0, the n. recursion level, there are pixels.
    const int rowstride = gdk_pixbuf_get_rowstride(this->pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(this->pixbuf);
    const int n_channels = gdk_pixbuf_get_n_channels(this->pixbuf);

    const guchar *p = pixels + x * rowstride + y * n_channels;
    const int r = p[0];
    const int g = p[1];
    const int b = p[2];

    //Do not store pixels as quadrants, just the average color of 4 pixel.
    double brightness = (static_cast<double>(r + g + b) / 3) / 255;
    return fabs(brightness) < 0.0001 ? 0.0001 : brightness;
}