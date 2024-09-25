#ifndef WFA_H
#define WFA_H

#include "rgbquadtree.h"
#include "quadrant.h"

#include <gtk/gtk.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>


/*void RGBWFACode(GtkWidget *clicked_button, gpointer file_chooser_button);
void RGBWFADecode(GtkWidget *clicked_button, gpointer file_chooser_button);*/

void GayWFACode(GtkWidget *clicked_button, gpointer file_chooser_button);
void GrayWFADecode(GtkWidget *clicked_button, gpointer file_chooser_button);

#endif //WFA_H
