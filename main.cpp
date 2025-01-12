#include <iostream>
#include <string>
#include <gtk/gtk.h>
#include <glib.h>
#include <math.h>
#include "header_files/DeterministicCoding.h"
#include "header_files/DeterministicDecoding.h"
#include "header_files/NondeterministicDecoding.h"

struct CodingArgs {
    GtkFileChooserButton *file_chooser_button;
    DeterministicCoding *code;
};

void DeterministicCodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *coding_button) {
    // Make coding button sensitive.
    gtk_widget_set_sensitive(coding_button, TRUE);
}

void DeterministicDecodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *decoding_button) {
    // Make decoding button sensitive.
    gtk_widget_set_sensitive(decoding_button, TRUE);
}

void NondeterministicDecodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *decoding_button) {
    // Make decoding button sensitive.
    gtk_widget_set_sensitive(decoding_button, TRUE);
}

void SwitchToDeterministicCoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "deterministic_coding_page");
}

void SwitchToDeterministicDeCoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "deterministic_decoding_page");
}

void SwitchToNondeterministicDeCoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "nondeterministic_decoding_page");
}

void onDeterministicCodingClicked(GtkWidget *clicked_button, gpointer user_data) {
    CodingArgs *data = static_cast<CodingArgs*>(user_data);

    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(data->file_chooser_button));
    //char *directory = g_path_get_dirname(filename);

    data->code = new DeterministicCoding(filename, 0.0001);
    //Coding code(filename, 0.0001);
    //code.Start();

    thread t1(&DeterministicCoding::Start, data->code);
    t1.detach();

    //g_free(filename);
    //g_free(directory);
}

void onDeterministicDecodingClicked(GtkWidget *clicked_button, GtkWidget *file_chooser_button) {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    char *directory = g_path_get_dirname(filename);

    DeterministicDecoding decode(filename, 9, 1);
    decode.Start(directory, "wfa_image.png");

    g_free(filename);
    g_free(directory);
}

void onNondeterministicDecodingClicked(GtkWidget *clicked_button, GtkWidget *file_chooser_button) {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    char *directory = g_path_get_dirname(filename);

    NondeterministicDecoding decode(filename, 9, 1);
    decode.Start(directory, "wfa_image.png");

    g_free(filename);
    g_free(directory);
}

int main(int argc, char *argv[]) {

    DeterministicCoding code;

    gtk_init(&argc, &argv);

    // Load the Glade file.
    GtkBuilder *builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "../View/mainWindow.glade", nullptr)) {
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

    GtkButton *deterministic_coding_page_button = GTK_BUTTON(gtk_builder_get_object(builder, "deterministic_coding_page_button"));
    GtkButton *deterministic_decoding_page_button = GTK_BUTTON(gtk_builder_get_object(builder, "deterministic_decoding_page_button"));
    GtkButton *nondeterministic_decoding_page_button = GTK_BUTTON(gtk_builder_get_object(builder, "nondeterministic_decoding_page_button"));

    GtkFileChooserButton *deterministic_coding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "deterministic_coding_file_chooser_button"));
    GtkFileChooserButton *deterministic_decoding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "deterministic_decoding_file_chooser_button"));
    GtkFileChooserButton *nondeterministic_decoding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "nondeterministic_decoding_file_chooser_button"));

    GtkWidget *deterministic_coding_button = GTK_WIDGET(gtk_builder_get_object(builder, "deterministic_coding_button"));
    GtkWidget *deterministic_decoding_button = GTK_WIDGET(gtk_builder_get_object(builder, "deterministic_decoding_button"));
    GtkWidget *nondeterministic_decoding_button = GTK_WIDGET(gtk_builder_get_object(builder, "nondeterministic_decoding_button"));

    // Function calling
    g_signal_connect(deterministic_coding_file_chooser_button, "file-set", G_CALLBACK(DeterministicCodingFileChoosed), deterministic_coding_button);
    g_signal_connect(deterministic_decoding_file_chooser_button, "file-set", G_CALLBACK(DeterministicDecodingFileChoosed), deterministic_decoding_button);
    g_signal_connect(nondeterministic_decoding_file_chooser_button, "file-set", G_CALLBACK(NondeterministicDecodingFileChoosed), nondeterministic_decoding_button);

    CodingArgs coding_args = {deterministic_coding_file_chooser_button, &code};

    g_signal_connect(deterministic_coding_button, "clicked", G_CALLBACK(onDeterministicCodingClicked), &coding_args);
    g_signal_connect(deterministic_decoding_button, "clicked", G_CALLBACK(onDeterministicDecodingClicked), deterministic_decoding_file_chooser_button);
    g_signal_connect(nondeterministic_decoding_button, "clicked", G_CALLBACK(onNondeterministicDecodingClicked), nondeterministic_decoding_file_chooser_button);

    g_signal_connect(deterministic_coding_page_button, "clicked", G_CALLBACK(SwitchToDeterministicCoding), stack);
    g_signal_connect(deterministic_decoding_page_button, "clicked", G_CALLBACK(SwitchToDeterministicDeCoding), stack);
    g_signal_connect(nondeterministic_decoding_page_button, "clicked", G_CALLBACK(SwitchToNondeterministicDeCoding), stack);

    gtk_builder_connect_signals(builder, nullptr);
    g_object_unref(builder);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}