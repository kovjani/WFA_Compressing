#ifndef QUADTREE_H
#define QUADTREE_H

#include "quadrant.h"

#include <stdbool.h>
#include <gdk-pixbuf/gdk-pixbuf-core.h>
#include <math.h>


bool isEndOfQuadtreeLevel(int n);
bool CompareQuadrantsColor(RGBQuadrant a, RGBQuadrant b, float epsilon);
void AverageColor(GdkPixbuf *pixbuf, RGBQuadrant *quadrant);
void CreateRGBQuadtree(GdkPixbuf *pixbuf, RGBQuadrant image, RGBQuadrant **quadtree, int quadtree_level);


#endif //QUADTREE_H
