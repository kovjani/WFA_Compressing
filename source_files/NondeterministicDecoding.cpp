#include "../header_files/NondeterministicDecoding.h"

NondeterministicDecoding::NondeterministicDecoding(char *filename, char *saved_filename, int depth, int initial_state) {
    this->depth = depth;
    this->decoding_image_size = 1 << depth; // The size of the image (2^res)

    this->directory = g_path_get_dirname(filename);
    this->saved_filename = saved_filename;

    this->pixels_colors = (double *)malloc(this->decoding_image_size*this->decoding_image_size*sizeof(double));

    string line, token;

    ifstream wfa_file(filename);

    // The first line is the size of the square matrices
    getline(wfa_file, line);
    this->n = stoi(line);

    getline(wfa_file, line);     // \n

    // Allocate memory
    this->I.resize(this->n);
    this->F.resize(this->n);

    this->Inn.resize(this->n, this->n);

    this->A.resize(this->n, this->n);
    this->B.resize(this->n, this->n);
    this->C.resize(this->n, this->n);
    this->D.resize(this->n, this->n);

    // Initialize I
    this->I.setIdentity();

    // Initialize the Inn, it's an nxn identity matrix for the first call.
    this->Inn.setIdentity();

    initial_state--; //the first state is the 0. index

    // Set I as an identity vector according to initial state.
    if(initial_state < I.size())
        for (int i = 0; i < I.size(); ++i) {
            I(i) = Inn(initial_state, i);
        }

    // Initialize F
    // The second line is the F (n*1 matrix), the average colors of states.
    {
        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int i = 0; i < this->n; ++i) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->F(i) = stod(token) * 255;
        }
    }

    getline(wfa_file, line);    // \n

    // Initialize matrix A
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int j = 0; j < this->n; ++j) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->A(i, j) = stod(token);
        }
    }

    getline(wfa_file, line);     // \n

    // Initialize matrix B
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int j = 0; j < this->n; ++j) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->B(i, j) = stod(token);
        }
    }

    getline(wfa_file, line);     // \n

    // Initialize matrix C
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int j = 0; j < this->n; ++j) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->C(i, j) = stod(token);
        }
    }

    getline(wfa_file, line);    // \n

    // Initialize matrix D
    for (int i = 0; i < this->n; ++i) {

        getline(wfa_file, line);
        stringstream line_stream(line);

        for (int j = 0; j < this->n; ++j) {
            getline(line_stream, token, ' '); // Split line by space and get the next token
            this->D(i, j) = stod(token);
        }
    }
}

NondeterministicDecoding::~NondeterministicDecoding() {
    // Free the allocated memory
    delete this->pixels_colors;
}

void NondeterministicDecoding::Start() {
    DecodePixelsColors(this->depth, 0, 0, this->I, this->Inn);
    char *saved_file_path = SaveDecodedImage(this->directory, this->saved_filename);

    OpenImage(saved_file_path);

    g_free(saved_file_path);
}

void NondeterministicDecoding::DecodePixelsColors(int level, int x, int y, const RowVectorXd &previous_result, const MatrixXd &next_matrix) {
    if(level > 0 ) {
        // Matrix multiplication
        RowVectorXd result = previous_result * next_matrix;

        int quadrant_size = 1 << level; // 2^level

        // Call recursively for all quadrants
        DecodePixelsColors(level-1, x, y, result, this->A);
        DecodePixelsColors(level-1, x + quadrant_size/2, y, result, this->B);
        DecodePixelsColors(level-1, x, y + quadrant_size/2, result, this->C);
        DecodePixelsColors(level-1, x + quadrant_size/2, y + quadrant_size/2,result, this->D);

        return;
    }
    // else
    // Pixels
    double average_color = previous_result * this->F;

    this->pixels_colors[x * this->decoding_image_size + y] = average_color;
}

void NondeterministicDecoding::OpenImage(char *full_path) const{
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *vbox;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Dekódolt kép");

    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);

    gtk_image_set_from_file(GTK_IMAGE(image), full_path);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();
}

char *NondeterministicDecoding::SaveDecodedImage(char *directory, char *filename) const {
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, this->decoding_image_size, this->decoding_image_size);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    for (int x = 0; x < this->decoding_image_size; ++x) {
        for (int y = 0; y < this->decoding_image_size; ++y) {
            guchar *p = pixels + x * rowstride + y * n_channels;
            p[0] = p[1] = p[2] = this->pixels_colors[x * this->decoding_image_size + y]; // Set R, G, B to the grayscale value
        }
    }

    char *filename_with_path = g_build_filename(directory, filename, NULL);

    gdk_pixbuf_save(pixbuf, filename_with_path, "png", NULL, NULL);

    g_object_unref(pixbuf);

    return filename_with_path;
}