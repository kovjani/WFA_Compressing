#ifndef QUADRANT_H
#define QUADRANT_H

typedef struct RGBQuadrant {
    int x;  //  The beginning x coordinate of the quadrant.
    int y;  //  The beginning y coordinate of the quadrant.
    int size;   //  The size of the quadrant.
    float r, g, b;  //  The average color of the quadrant (r, g, b).
} RGBQuadrant;

/*typedef struct GrayQuadrant {
    int x;  //  The beginning x coordinate of the quadrant.
    int y;  //  The beginning y coordinate of the quadrant.
    int size;   //  The size of the quadrant.
    float gray;  //  The average gray color of the quadrant.
} GrayQuadrant;*/

#endif //QUADRANT_H
