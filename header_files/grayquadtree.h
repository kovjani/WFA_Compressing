#ifndef GRAYQUADTREE_H
#define GRAYQUADTREE_H

#include <gtk/gtk.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include <math.h>

void push(double *stack, double value);
double pop(double *stack);
double CreateQuadtree(GdkPixbuf *pixbuf, double *quadtree, int level, int index, int x, int y);
void WFACode(GtkWidget *clicked_button, gpointer file_chooser_button);

#endif //GRAYQUADTREE_H
