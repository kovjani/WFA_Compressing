#include "../header_files/rgbquadtree.h"

bool isEndOfQuadtreeLevel(int n) {
    if (n < 1) return false;
    float fourth_log = log(n)/log(4);
    return (int)fourth_log == fourth_log;
}

bool CompareQuadrantsColor(RGBQuadrant a, RGBQuadrant b, float epsilon) {
    // Compare the colors of the quadrants allow some approximation error (epsilon).
    // Call CreateQuadtree() recursively while a quadrant can be divide other quadrants with different colors.

    // if abs(a - b) < epsilon => true

    // Red
    if(fabs(a.r - b.r) >= epsilon) {
        return false;
    }

    // Green
    if(fabs(a.g - b.g) >= epsilon) {
        return false;
    }

    // Blue
    if(fabs(a.b - b.b) >= epsilon) {
        return false;
    }

    return true;
}

void AverageColor(GdkPixbuf *pixbuf, RGBQuadrant *quadrant){
    // Count the average color of the image segment (quadrant) according to parameters.
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    quadrant->r = 0;
    quadrant->g = 0;
    quadrant->b = 0;

    // Sum the colors of the quadrant
    for (int y = quadrant->y; y < quadrant->y + quadrant->size; y++) {
        for (int x = quadrant->x; x < quadrant->x + quadrant->size; x++) {
            guchar *p = pixels + y * rowstride + x * n_channels;
            guchar red = p[0];
            guchar green = p[1];
            guchar blue = p[2];

            quadrant->r += red;
            quadrant->g += green;
            quadrant->b += blue;
        }
    }

    // Divide the sum of the colors by the size of the quadrant (pixels).
    quadrant->r /= quadrant->size*quadrant->size;
    quadrant->g /= quadrant->size*quadrant->size;
    quadrant->b /= quadrant->size*quadrant->size;
}

void CreateRGBQuadtree(GdkPixbuf *pixbuf, RGBQuadrant image, RGBQuadrant **quadtree, int quadtree_level){
    // image variable is a full image or a quadrant of an image which will be splited into 4 quadrants (q1, q2, q3, q4).

    // First quadrant
    RGBQuadrant q1;
    q1.size = image.size / 2;
    q1.x = image.x;
    q1.y = image.y;
    AverageColor(pixbuf, &q1);

    // Second quadrant
    RGBQuadrant q2;
    q2.size = image.size / 2;
    q2.x = image.x + q2.size;
    q2.y = image.y;
    AverageColor(pixbuf, &q2);

    // Third quadrant
    RGBQuadrant q3;
    q3.size = image.size / 2;
    q3.x = image.x;
    q3.y = image.y + q3.size;
    AverageColor(pixbuf, &q3);

    // Fourth quadrant
    RGBQuadrant q4;
    q4.size = image.size / 2;
    q4.x = image.x + q4.size;
    q4.y = image.y + q4.size;
    AverageColor(pixbuf, &q4);

    float epsilon = 10.0f;

    // If the quadrant is a pixel or the quadrants have the same color, don't call the function recursively.
    // If the quadrant is not a pixel (the size is bigger than 1) and the quadrants don't have the same color, call this function recursively.
    if( image.size / 2 > 1 &&
        (!CompareQuadrantsColor(q1, q2, epsilon) ||
        !CompareQuadrantsColor(q1, q3, epsilon) ||
        !CompareQuadrantsColor(q1, q4, epsilon))){
        // Divide quadrants to other quadrants.
        CreateRGBQuadtree(pixbuf, q1, quadtree, quadtree_level + 1);
        CreateRGBQuadtree(pixbuf, q2, quadtree, quadtree_level + 1);
        CreateRGBQuadtree(pixbuf, q3, quadtree, quadtree_level + 1);
        CreateRGBQuadtree(pixbuf, q4, quadtree, quadtree_level + 1);
    }


    // g_print("level: %d, size: %d\n", quadtree_level, image.size);

    quadtree[quadtree_level + 0] = (RGBQuadrant *)malloc(sizeof(RGBQuadrant));
    quadtree[quadtree_level + 0]->x = q1.x;
    quadtree[quadtree_level + 0]->y = q1.y;
    quadtree[quadtree_level + 0]->size = q1.size;
    quadtree[quadtree_level + 0]->r = q1.r;
    quadtree[quadtree_level + 0]->g = q1.g;
    quadtree[quadtree_level + 0]->b = q1.b;

    quadtree[quadtree_level + 1] = (RGBQuadrant *)malloc(sizeof(RGBQuadrant));
    quadtree[quadtree_level + 1]->x = q2.x;
    quadtree[quadtree_level + 1]->y = q2.y;
    quadtree[quadtree_level + 1]->size = q2.size;
    quadtree[quadtree_level + 1]->r = q2.r;
    quadtree[quadtree_level + 1]->g = q2.g;
    quadtree[quadtree_level + 1]->b = q2.b;

    quadtree[quadtree_level + 2] = (RGBQuadrant *)malloc(sizeof(RGBQuadrant));
    quadtree[quadtree_level + 2]->x = q3.x;
    quadtree[quadtree_level + 2]->y = q3.y;
    quadtree[quadtree_level + 2]->size = q3.size;
    quadtree[quadtree_level + 2]->r = q3.r;
    quadtree[quadtree_level + 2]->g = q3.g;
    quadtree[quadtree_level + 2]->b = q3.b;

    quadtree[quadtree_level + 3] = (RGBQuadrant *)malloc(sizeof(RGBQuadrant));
    quadtree[quadtree_level + 3]->x = q4.x;
    quadtree[quadtree_level + 3]->y = q4.y;
    quadtree[quadtree_level + 3]->size = q4.size;
    quadtree[quadtree_level + 3]->r = q4.r;
    quadtree[quadtree_level + 3]->g = q4.g;
    quadtree[quadtree_level + 3]->b = q4.b;
}