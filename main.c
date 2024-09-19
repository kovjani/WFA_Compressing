#include <stdio.h>
#include <gtk/gtk.h>

typedef struct Quadrant {
    int x;  //  The beginning x coordinate of the quadrant.
    int y;  //  The beginning y coordinate of the quadrant.
    int size;   //  The size of the quadrant.
    float *color;  //  The average color of the quadrant (r, g, b).
} Quadrant;

void Close(GtkWidget *widget, gpointer data) {
    gtk_window_close(widget);
}

void AverageCounter(GdkPixbuf *pixbuf, Quadrant *quadrant) {

    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    // Count the average color of the image segment (quadrant) according to parameters.

    quadrant->color[0] = 0;
    quadrant->color[1] = 0;
    quadrant->color[2] = 0;

    // Sum the colors of the quadrant
    for (int y = quadrant->y; y < quadrant->y + quadrant->size; y++) {
        for (int x = quadrant->x; x < quadrant->x + quadrant->size; x++) {
            guchar *p = pixels + y * rowstride + x * n_channels;
            guchar red = p[0];
            guchar green = p[1];
            guchar blue = p[2];

            quadrant->color[0] += red;
            quadrant->color[1] += green;
            quadrant->color[2] += blue;
        }
    }

    // Divide the sum of the colors by the size of the quadrant (pixels).
    quadrant->color[0] /= quadrant->size*quadrant->size;
    quadrant->color[1] /= quadrant->size*quadrant->size;
    quadrant->color[2] /= quadrant->size*quadrant->size;
}

int FileCompressing(GtkWidget *clicked_button, gpointer file_chooser_button) {
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
            return 1;
        }

        // Compare pixel colors and create automata.

        int width = gdk_pixbuf_get_width(pixbuf);
        int height = gdk_pixbuf_get_height(pixbuf);

        Quadrant full_image;
        full_image.x = 0;
        full_image.y = 0;
        full_image.size = width > height ? height : width;
        full_image.color = malloc(3 * sizeof(float));

        AverageCounter(pixbuf, &full_image);

        g_print("%d, %d, %d, %f, %f, %f\n", full_image.x, full_image.y, full_image.size, full_image.color[0], full_image.color[1], full_image.color[2]);

        // Free the allocated memory.
        g_object_unref(pixbuf);
        g_free(full_image.color);
        g_free(filename);

        return 0;
    }
}

void OnFileChoosed(GtkFileChooserButton *clicked_button, gpointer compress_button) {
    // Make compress button sensitive.
    gtk_widget_set_sensitive(compress_button, TRUE);
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder, *window;
    GtkFileChooserButton *fileChooser_button;
    GtkWidget *compress_button;

    gtk_init(&argc, &argv);

    // Load the Glade file.
    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "../View/mainWindow.glade", NULL)) {
        g_printerr("Error loading mainWindow.glade\n");
        return 1;
    }

    // Get the main window pointer from the Glade file.
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    if (!window) {
        g_printerr("Error: could not find the 'window' object\n");
        return 1;
    }

    // Get buttons pointer from the Glade file.
    fileChooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "fileChooserButton"));
    compress_button = GTK_WIDGET(gtk_builder_get_object(builder, "compressButton"));

    // After a file is selected, call the OnFileChoosed function.
    g_signal_connect(fileChooser_button, "file-set", G_CALLBACK(OnFileChoosed), compress_button);
    // Call FileCompressing function on compress button clicked.
    g_signal_connect(compress_button, "clicked", G_CALLBACK(FileCompressing), fileChooser_button);


    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}