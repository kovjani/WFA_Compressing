#include <stdio.h>
#include <gtk/gtk.h>

void Close(GtkWidget *widget, gpointer data) {
    gtk_window_close(widget);
}

void FileCompressing(GtkWidget *clickedButton, gpointer fileChooserButton) {
    // Get the selected file name.
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooserButton));
    if (filename) {
        // Process the file.
        g_print("Selected file: %s\n", filename);

        // Free the filename string.
        g_free(filename);
    }
}

void OnFileChoosed(GtkFileChooserButton *clickedButton, gpointer compressButton) {
    // Make compress button sensitive.
    gtk_widget_set_sensitive(compressButton, TRUE);
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder, *window;
    GtkFileChooserButton *fileChooserButton;
    GtkWidget *compressButton;

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
    fileChooserButton = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "fileChooserButton"));
    compressButton = GTK_WIDGET(gtk_builder_get_object(builder, "compressButton"));

    // After a file is selected, call the OnFileChoosed function.
    g_signal_connect(fileChooserButton, "file-set", G_CALLBACK(OnFileChoosed), compressButton);
    // Call FileCompressing function on compress button clicked.
    g_signal_connect(compressButton, "clicked", G_CALLBACK(FileCompressing), fileChooserButton);


    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}