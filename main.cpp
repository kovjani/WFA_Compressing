#include <iostream>
#include <string>
#include <gtk/gtk.h>
#include <math.h>
#include "header_files/Coding.h"
#include "header_files/Decoding.h"
#include "header_files/Testing.h"

void CodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *coding_button) {
    // Make compress button sensitive.
    gtk_widget_set_sensitive(coding_button, TRUE);
}

void DecodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *decoding_button) {
    // Make compress button sensitive.
    gtk_widget_set_sensitive(decoding_button, TRUE);
}

void SwitchToCoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "coding_page");
}

void SwitchToDeCoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "decoding_page");
}

void onCodingClicked(GtkWidget *clicked_button, GtkWidget *file_chooser_button) {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    char *directory = g_path_get_dirname(filename);

    Coding code(filename);
    code.Start();

    g_free(filename);
    g_free(directory);
}

void onDecodingClicked(GtkWidget *clicked_button, GtkWidget *file_chooser_button) {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    char *directory = g_path_get_dirname(filename);

    Decoding decode(filename, 9);
    decode.Start(directory, "wfa_image");

    g_free(filename);
    g_free(directory);
}

int main(int argc, char *argv[]) {
    /*Testing test(1,9);
    test.Start();
    */
    gtk_init(&argc, &argv);

    // Load the Glade file.
    GtkBuilder *builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "../View/mainWindow.glade", NULL)) {
        g_printerr("Error loading mainWindow.glade\n");
        return 1;
    }

    // Get the main window pointer from the Glade file.
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    if (!window) {
        g_printerr("Error: could not find the 'window' object\n");
        return 1;
    }

    // Get widgets pointer from the Glade file.
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));

    GtkButton *coding_page_button = GTK_BUTTON(gtk_builder_get_object(builder, "coding_page_button"));
    GtkButton *decoding_page_button = GTK_BUTTON(gtk_builder_get_object(builder, "decoding_page_button"));

    GtkFileChooserButton *coding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "coding_file_chooser_button"));
    GtkFileChooserButton *decoding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "decoding_file_chooser_button"));

    GtkWidget *coding_button = GTK_WIDGET(gtk_builder_get_object(builder, "coding_button"));
    GtkWidget *decoding_button = GTK_WIDGET(gtk_builder_get_object(builder, "decoding_button"));

    // Function calling
    g_signal_connect(coding_file_chooser_button, "file-set", G_CALLBACK(CodingFileChoosed), coding_button);
    g_signal_connect(decoding_file_chooser_button, "file-set", G_CALLBACK(DecodingFileChoosed), decoding_button);

    g_signal_connect(coding_button, "clicked", G_CALLBACK(onCodingClicked), coding_file_chooser_button);
    g_signal_connect(decoding_button, "clicked", G_CALLBACK(onDecodingClicked), decoding_file_chooser_button);

    g_signal_connect(coding_page_button, "clicked", G_CALLBACK(SwitchToCoding), stack);
    g_signal_connect(decoding_page_button, "clicked", G_CALLBACK(SwitchToDeCoding), stack);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}