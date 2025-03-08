#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <cstring>
#include <gtk/gtk.h>
#include <glib.h>
#include <math.h>
#include "header_files/DeterministicCoding.h"
#include "header_files/DeterministicDecoding.h"
#include "header_files/NondeterministicDecoding.h"
#include "header_files/NondeterministicCoding.h"

static GtkWidget *window = nullptr;
static GtkWidget *spinner = nullptr;

static GtkWidget *deterministic_coding_page_button = nullptr;
static GtkWidget *deterministic_decoding_page_button = nullptr;
static GtkWidget *nondeterministic_coding_page_button = nullptr;
static GtkWidget *nondeterministic_decoding_page_button = nullptr;

static GtkSpinButton *spin_depth_dd = nullptr, *spin_state_dd = nullptr, *spin_depth_nc = nullptr;
static GtkSpinButton *spin_depth_nd = nullptr, *spin_state_nd = nullptr;
static GtkComboBoxText *combo_resolution_nd = nullptr, *combo_resolution_dd = nullptr, *combo_detail_nc = nullptr;

char* RemoveExtension(char* filename) {
    char* last_dot = strrchr(filename, '.');
    if (last_dot == nullptr) {
        // No extension found
        return filename;
    }
    *last_dot = '\0';
    return filename;
}

void SetWindowSensitivity(GtkWidget &clicked_button, GtkWidget &file_chooser_button, bool value) {
    if(value) {
        gtk_spinner_stop(GTK_SPINNER(spinner));
    } else {
        gtk_spinner_start(GTK_SPINNER(spinner));
    }

    gtk_widget_set_visible(spinner, !value);
    gtk_widget_set_sensitive(&clicked_button, value);
    gtk_widget_set_sensitive(&file_chooser_button, value);

    gtk_widget_set_sensitive(deterministic_coding_page_button, value);
    gtk_widget_set_sensitive(deterministic_decoding_page_button, value);
    gtk_widget_set_sensitive(nondeterministic_coding_page_button, value);
    gtk_widget_set_sensitive(nondeterministic_decoding_page_button, value);
}

void DeterministicCodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *coding_button) {
    gtk_widget_set_sensitive(coding_button, TRUE);
}

void DeterministicDecodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *decoding_button) {
    gtk_widget_set_sensitive(decoding_button, TRUE);
}

void NondeterministicCodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *coding_button) {
    gtk_widget_set_sensitive(coding_button, TRUE);
}

void NondeterministicDecodingFileChoosed(GtkFileChooserButton *clicked_button, GtkWidget *decoding_button) {
    gtk_widget_set_sensitive(decoding_button, TRUE);
}

void SwitchToDeterministicCoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "deterministic_coding_page");
}

void SwitchToDeterministicDecoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "deterministic_decoding_page");
}

void SwitchToNondeterministicCoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "nondeterministic_coding_page");
}

void SwitchToNondeterministicDecoding(GtkButton *button, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "nondeterministic_decoding_page");
}

// an atomic variable can be used in multiple threads
std::atomic<bool> running(true);

// Process GTK events to keep the UI responsive
void gtk_main_loop() {
    while (running) {
        while (gtk_events_pending())
            gtk_main_iteration();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void onDeterministicCodingClicked(GtkWidget *clicked_button, GtkWidget *file_chooser_button) {

    SetWindowSensitivity(*clicked_button, *file_chooser_button, FALSE);

    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    char *basename = g_path_get_basename(filename);
    char *basename_without_extension = RemoveExtension(basename);

    DeterministicCoding code(filename,basename_without_extension, 0.0001);
    strcat(basename_without_extension, ".dwfa");

    running = true;

    std::thread t([&code]() {
        code.Start();
        running = false;
    });

    gtk_main_loop();

    t.join();

    g_free(filename);
    g_free(basename);

    SetWindowSensitivity(*clicked_button, *file_chooser_button, TRUE);
}

void onDeterministicDecodingClicked(GtkWidget *clicked_button, GtkWidget *file_chooser_button) {

    SetWindowSensitivity(*clicked_button, *file_chooser_button, FALSE);

    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));

    int decoding_depth = 9;
    char* resolution = gtk_combo_box_text_get_active_text(combo_resolution_dd);
    int initial_state = static_cast<int>(gtk_spin_button_get_value(spin_state_dd));

    if(strcmp(resolution, "128x128") == 0) {
        decoding_depth = 7;
    } else if(strcmp(resolution, "256x256") == 0) {
        decoding_depth = 8;
    } else if(strcmp(resolution, "512x512") == 0) {
        decoding_depth = 9;
    } else if(strcmp(resolution, "1024x1024") == 0) {
        decoding_depth = 10;
    } else if(strcmp(resolution, "2048x2048") == 0) {
        decoding_depth = 11;
    }

    DeterministicDecoding decode(filename, "deterministic_decoded_image.png", decoding_depth, initial_state);
    running = true;

    std::thread t([&decode]() {
        decode.Start();
        running = false;
    });

    gtk_main_loop();

    t.join();

    g_free(filename);

    SetWindowSensitivity(*clicked_button, *file_chooser_button, TRUE);
}

void onNondeterministicCodingClicked(GtkWidget *clicked_button, GtkWidget *file_chooser_button) {
    SetWindowSensitivity(*clicked_button, *file_chooser_button, FALSE);

    // filename contains path.
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    char *basename = g_path_get_basename(filename);
    char *basename_without_extension = RemoveExtension(basename);

    int coding_depth = 4;
    char* gtk_details = gtk_combo_box_text_get_active_text(combo_detail_nc);

    if(strcmp(gtk_details, "1    (4)") == 0) {
        coding_depth = 1;
    } else if(strcmp(gtk_details, "2    (16)") == 0) {
        coding_depth = 2;
    } else if(strcmp(gtk_details, "3    (64)") == 0) {
        coding_depth = 3;
    } else if(strcmp(gtk_details, "4    (256)") == 0) {
        coding_depth = 4;
    } else if(strcmp(gtk_details, "5    (1024)") == 0) {
        coding_depth = 5;
    } else if(strcmp(gtk_details, "6    (4096)") == 0) {
        coding_depth = 6;
    }

    NondeterministicCoding code(filename, basename_without_extension, coding_depth, 0.000001);
    strcat(basename_without_extension, ".ndwfa");

    running = true;

    std::thread t([&code]() {
        code.Start();
        running = false;
    });

    gtk_main_loop();

    t.join();

    g_free(filename);
    g_free(basename);

    SetWindowSensitivity(*clicked_button, *file_chooser_button, TRUE);
}

void onNondeterministicDecodingClicked(GtkWidget *clicked_button, GtkWidget *file_chooser_button) {

    SetWindowSensitivity(*clicked_button, *file_chooser_button, FALSE);

    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    char *directory = g_path_get_dirname(filename);

    int decoding_depth = 9;
    char* resolution = gtk_combo_box_text_get_active_text(combo_resolution_nd);
    int initial_state = static_cast<int>(gtk_spin_button_get_value(spin_state_nd));

    if(strcmp(resolution, "128x128") == 0) {
        decoding_depth = 7;
    } else if(strcmp(resolution, "256x256") == 0) {
        decoding_depth = 8;
    } else if(strcmp(resolution, "512x512") == 0) {
        decoding_depth = 9;
    } else if(strcmp(resolution, "1024x1024") == 0) {
        decoding_depth = 10;
    } else if(strcmp(resolution, "2048x2048") == 0) {
        decoding_depth = 11;
    }

    NondeterministicDecoding decode(filename, "nondeterministic_decoded_image.png", decoding_depth, initial_state);
    running = true;

    std::thread t([&decode]() {
        decode.Start();
        running = false;
    });

    gtk_main_loop();

    t.join();

    g_free(filename);
    g_free(directory);

    SetWindowSensitivity(*clicked_button, *file_chooser_button, TRUE);
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
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    if (!window) {
        g_printerr("Error: could not find the 'window' object\n");
        return 1;
    }

    spinner = GTK_WIDGET(gtk_builder_get_object(builder, "spinner"));

    // Get widgets pointer from the Glade file.
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));

    spin_depth_dd = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_depth_dd"));
    spin_state_dd = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_state_dd"));
    spin_depth_nc = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_depth_nc"));
    spin_depth_nd = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_depth_nd"));
    spin_state_nd = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_state_nd"));

    combo_resolution_nd = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_resolution_nd"));
    combo_resolution_dd = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_resolution_dd"));
    combo_detail_nc = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_detail_nc"));

    deterministic_coding_page_button = GTK_WIDGET(gtk_builder_get_object(builder, "deterministic_coding_page_button"));
    deterministic_decoding_page_button = GTK_WIDGET(gtk_builder_get_object(builder, "deterministic_decoding_page_button"));
    nondeterministic_coding_page_button = GTK_WIDGET(gtk_builder_get_object(builder, "nondeterministic_coding_page_button"));
    nondeterministic_decoding_page_button = GTK_WIDGET(gtk_builder_get_object(builder, "nondeterministic_decoding_page_button"));

    GtkFileChooserButton *deterministic_coding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "deterministic_coding_file_chooser_button"));
    GtkFileChooserButton *deterministic_decoding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "deterministic_decoding_file_chooser_button"));
    GtkFileChooserButton *nondeterministic_coding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "nondeterministic_coding_file_chooser_button"));
    GtkFileChooserButton *nondeterministic_decoding_file_chooser_button = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "nondeterministic_decoding_file_chooser_button"));

    GtkWidget *deterministic_coding_button = GTK_WIDGET(gtk_builder_get_object(builder, "deterministic_coding_button"));
    GtkWidget *deterministic_decoding_button = GTK_WIDGET(gtk_builder_get_object(builder, "deterministic_decoding_button"));
    GtkWidget *nondeterministic_coding_button = GTK_WIDGET(gtk_builder_get_object(builder, "nondeterministic_coding_button"));
    GtkWidget *nondeterministic_decoding_button = GTK_WIDGET(gtk_builder_get_object(builder, "nondeterministic_decoding_button"));

    // Function calling
    g_signal_connect(deterministic_coding_file_chooser_button, "file-set", G_CALLBACK(DeterministicCodingFileChoosed), deterministic_coding_button);
    g_signal_connect(deterministic_decoding_file_chooser_button, "file-set", G_CALLBACK(DeterministicDecodingFileChoosed), deterministic_decoding_button);
    g_signal_connect(nondeterministic_coding_file_chooser_button, "file-set", G_CALLBACK(NondeterministicCodingFileChoosed), nondeterministic_coding_button);
    g_signal_connect(nondeterministic_decoding_file_chooser_button, "file-set", G_CALLBACK(NondeterministicDecodingFileChoosed), nondeterministic_decoding_button);

    g_signal_connect(deterministic_coding_button, "clicked", G_CALLBACK(onDeterministicCodingClicked), deterministic_coding_file_chooser_button);
    g_signal_connect(deterministic_decoding_button, "clicked", G_CALLBACK(onDeterministicDecodingClicked), deterministic_decoding_file_chooser_button);
    g_signal_connect(nondeterministic_coding_button, "clicked", G_CALLBACK(onNondeterministicCodingClicked), nondeterministic_coding_file_chooser_button);
    g_signal_connect(nondeterministic_decoding_button, "clicked", G_CALLBACK(onNondeterministicDecodingClicked), nondeterministic_decoding_file_chooser_button);

    g_signal_connect(deterministic_coding_page_button, "clicked", G_CALLBACK(SwitchToDeterministicCoding), stack);
    g_signal_connect(deterministic_decoding_page_button, "clicked", G_CALLBACK(SwitchToDeterministicDecoding), stack);
    g_signal_connect(nondeterministic_coding_page_button, "clicked", G_CALLBACK(SwitchToNondeterministicCoding), stack);
    g_signal_connect(nondeterministic_decoding_page_button, "clicked", G_CALLBACK(SwitchToNondeterministicDecoding), stack);

    gtk_builder_connect_signals(builder, nullptr);
    g_object_unref(builder);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}