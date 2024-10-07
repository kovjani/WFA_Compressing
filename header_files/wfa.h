#ifndef WFA_H
#define WFA_H

#include "rgbquadtree.h"
#include "quadrant.h"

#include <gtk/gtk.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*void RGBWFACode(GtkWidget *clicked_button, gpointer file_chooser_button);
void RGBWFADecode(GtkWidget *clicked_button, gpointer file_chooser_button);*/

void GayWFACode(GtkWidget *clicked_button, gpointer file_chooser_button);
void GrayWFADecode(GtkWidget *clicked_button, gpointer file_chooser_button);
void Test2();
void Test1();

#endif //WFA_H
