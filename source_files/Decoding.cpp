#include "../header_files/Decoding.h"

Decoding::Decoding(char *filename, char *saved_filename, int depth) {
    this->depth = depth;
    this->decoding_image_size = 1 << depth; // The size of the image (2^depth)

    this->directory = g_path_get_dirname(filename);
    this->saved_filename = saved_filename;

    this->image_pixels = new uint8_t[decoding_image_size * decoding_image_size];
}

Decoding::~Decoding() {
    delete[] this->image_pixels;
    this->image_pixels = nullptr;

    g_free(this->directory);
}


void Decoding::OpenImage(char *full_path) {

    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *vbox;

    //gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Dekódolt kép");

    vbox = gtk_vbox_new(false, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image, true, true, 0);

    gtk_image_set_from_file(GTK_IMAGE(image), full_path);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    gtk_widget_show_all(window);

    gtk_main();
}

/*char *Decoding::SaveDecodedImage(char *directory, char *filename) const {

    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, this->decoding_image_size, this->decoding_image_size);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    for (int x = 0; x < this->decoding_image_size; ++x) {
        for (int y = 0; y < this->decoding_image_size; ++y) {
            guchar *p = pixels + y * rowstride + x * n_channels;
            p[0] = p[1] = p[2] = this->image_pixels[x * this->decoding_image_size + y]; // Set R, G, B to the grayscale value
        }
    }
    char *filename_with_path = g_build_filename(directory, filename, nullptr);

    gdk_pixbuf_save(pixbuf, filename_with_path, "png", nullptr, nullptr);

    g_object_unref(pixbuf);

    return filename_with_path;
}*/

char *Decoding::SaveDecodedImage(char *directory, char *filename) const {

    uint8_t* image_pixels_rgb = new uint8_t[this->decoding_image_size * this->decoding_image_size * 3];
    for (int i = 0; i < this->decoding_image_size * this->decoding_image_size; ++i) {
        image_pixels_rgb[i*3] = image_pixels[i];
        image_pixels_rgb[i*3+1] = image_pixels[i];
        image_pixels_rgb[i*3+2] = image_pixels[i];
    }
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data(
        image_pixels_rgb,
        GDK_COLORSPACE_RGB,
        FALSE,
        8,
        this->decoding_image_size,
        this->decoding_image_size,
        this->decoding_image_size * 3,
        nullptr,
        nullptr
    );

    char *filename_with_path = g_build_filename(directory, filename, nullptr);

    gdk_pixbuf_save(pixbuf, filename_with_path, "png", nullptr, nullptr);

    g_object_unref(pixbuf);

    return filename_with_path;
}