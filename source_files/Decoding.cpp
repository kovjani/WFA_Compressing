#include "../header_files/Decoding.h"

Decoding::Decoding(char *filename, int depth){
    this->depth = depth;
    this->decoding_image_size = 1 << depth; // The size of the image (2^res)

    this->pixels_colors = (double *)malloc(this->decoding_image_size*this->decoding_image_size*sizeof(double));

    // Open a file in read mode
    FILE *wfa_file = fopen(filename, "r");

    // Check if the file was opened successfully
    if (wfa_file == NULL) {
        g_print("Failed to open the file.\n");
        return;
    }

    char line[256];

    // The first line is the size of the square matrices
    fgets(line, sizeof(line), wfa_file);
    this->n = atoi(line);

    fgets(line, sizeof(line), wfa_file);     // \n



    // Allocate memory for the matrices
    this->I = gsl_matrix_alloc(1, this->n);
    this->F = gsl_matrix_alloc(this->n, 1);

    this->Inn = gsl_matrix_alloc(this->n, this->n);

    this->A = gsl_matrix_alloc(this->n, this->n);
    this->B = gsl_matrix_alloc(this->n, this->n);
    this->C = gsl_matrix_alloc(this->n, this->n);
    this->D = gsl_matrix_alloc(this->n, this->n);

    // Initialize I
    gsl_matrix_set_identity(this->I);

    // Initialize the Inn, it's an nxn identity matrix for the first call.
    gsl_matrix_set_identity(this->Inn);

    // Initialize F
    // The second line is the F (n*1 matrix), the average colors of states.
    {
        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        for (int i = 0; i < this->n; ++i) {
            gsl_matrix_set(this->F, i, 0, atof(token));
            token = strtok(NULL, " "); // Get the next token
        }

        g_free(token);
    }
    fgets(line, sizeof(line), wfa_file);     // \n

    // Initialize matrix A
    for (int i = 0; i < this->n; ++i) {

        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        for (int j = 0; j < this->n; ++j) {
            gsl_matrix_set(this->A, i, j, atof(token));
            token = strtok(NULL, " "); // Get the next token
        }

        g_free(token);
    }

    fgets(line, sizeof(line), wfa_file);     // \n

    // Initialize matrix B
    for (int i = 0; i < this->n; ++i) {

        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        for (int j = 0; j < this->n; ++j) {
            gsl_matrix_set(this->B, i, j, atof(token));
            token = strtok(NULL, " "); // Get the next token
        }

        g_free(token);
    }

    fgets(line, sizeof(line), wfa_file);     // \n

    // Initialize matrix C
    for (int i = 0; i < this->n; ++i) {

        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        for (int j = 0; j < this->n; ++j) {
            gsl_matrix_set(this->C, i, j, atof(token));
            token = strtok(NULL, " "); // Get the next token
        }

        g_free(token);
    }

    fgets(line, sizeof(line), wfa_file);     // \n

    // Initialize matrix D
    for (int i = 0; i < this->n; ++i) {

        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        for (int j = 0; j < this->n; ++j) {
            gsl_matrix_set(this->D, i, j, atof(token));
            token = strtok(NULL, " "); // Get the next token
        }

        g_free(token);
    }
}

Decoding::Decoding(gsl_matrix *a, gsl_matrix *b, gsl_matrix *c, gsl_matrix *d, gsl_matrix *f, int n, int depth, bool testing)
    : n(n), depth(depth), testing(testing) {

    // Allocate memory for the matrices
    this->I = gsl_matrix_alloc(1, this->n);
    this->F = gsl_matrix_alloc(this->n, 1);

    this->Inn = gsl_matrix_alloc(this->n, this->n);

    this->A = gsl_matrix_alloc(this->n, this->n);
    this->B = gsl_matrix_alloc(this->n, this->n);
    this->C = gsl_matrix_alloc(this->n, this->n);
    this->D = gsl_matrix_alloc(this->n, this->n);

    // Initialize matrices
    gsl_matrix_set_identity(this->I);
    gsl_matrix_set_identity(this->Inn);

    gsl_matrix_memcpy(this->A, a);
    gsl_matrix_memcpy(this->B, b);
    gsl_matrix_memcpy(this->C, c);
    gsl_matrix_memcpy(this->D, d);
    gsl_matrix_memcpy(this->F, f);

    this->decoding_image_size = 1 << depth; // The size of the image (2^res)
    this->pixels_colors = (double *)malloc(decoding_image_size*decoding_image_size*sizeof(double));
}

Decoding::~Decoding() {
    // Free the allocated memory
    gsl_matrix_free(this->A);
    gsl_matrix_free(this->B);
    gsl_matrix_free(this->C);
    gsl_matrix_free(this->D);

    gsl_matrix_free(this->Inn);

    gsl_matrix_free(this->I);
    gsl_matrix_free(this->F);

    g_free(this->pixels_colors);
}

/*int Decoding::getN() const {
    return this->n;
}
int Decoding::getDepth() const {
    return this->depth;
}
gsl_matrix *Decoding::getA() const {
    return this->A;
}
gsl_matrix *Decoding::getB() const {
    return this->B;
}
gsl_matrix *Decoding::getC() const {
    return this->C;
}
gsl_matrix *Decoding::getD() const {
    return this->D;
}
gsl_matrix *Decoding::getF() const {
    return this->F;
}*/

void Decoding::Start(char *directory, char *saved_filename) {
    DecodePixelsColors(this->depth, 0, 0, this->I, this->Inn);
    char *saved_file_path = SaveDecodedImage(directory, saved_filename);

    if(!this->testing) {
        OpenImage(saved_file_path);
    }

    g_free(saved_file_path);
}

void Decoding::OpenImage(char *full_path) const{
    
    GtkWidget *window;
    GtkWidget *image;
    GtkWidget *vbox;

    //gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image Loader");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);

    gtk_image_set_from_file(GTK_IMAGE(image), full_path);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();
}

char *Decoding::SaveDecodedImage(char *directory, char *filename) const {

    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, this->decoding_image_size, this->decoding_image_size);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    for (int y = 0; y < this->decoding_image_size; ++y) {
        for (int x = 0; x < this->decoding_image_size; ++x) {
            guchar *p = pixels + y * rowstride + x * n_channels;
            p[0] = p[1] = p[2] = this->pixels_colors[y * this->decoding_image_size + x]; // Set R, G, B to the grayscale value
        }
    }
    char *filename_with_path = g_build_filename(directory, filename, NULL);

    gdk_pixbuf_save(pixbuf, filename_with_path, "png", NULL, NULL);

    g_object_unref(pixbuf);

    return filename_with_path;
}

double Decoding::DecodePixelsColors(int level, int x, int y, gsl_matrix *previous_matrix, gsl_matrix *next_matrix) {

    gsl_matrix *result = gsl_matrix_alloc(1, this->n);

    // matrix multiplication
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, previous_matrix, next_matrix, 0.0, result);

    int quadrant_size = 1 << level; // 2^level

    if(level > 0) {
        double a = DecodePixelsColors(level-1, x, y, result, this->A);
        double b = DecodePixelsColors(level-1, x + (int)(quadrant_size/2), y, result, this->B);
        double c = DecodePixelsColors(level-1, x, y + (int)(quadrant_size/2), result, this->C);
        double d = DecodePixelsColors(level-1, x + (int)(quadrant_size/2), y + (int)(quadrant_size/2),result, this->D);

        gsl_matrix_free(result);

        return (a+b+c+d)/4;
    }
    // else
    // Pixels
    gsl_matrix *average_color = gsl_matrix_alloc(1, 1);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, result, this->F, 0.0, average_color);

    this->pixels_colors[y * this->decoding_image_size + x] = gsl_matrix_get(average_color, 0, 0);

    double avg_color = gsl_matrix_get(average_color, 0, 0);

    gsl_matrix_free(average_color);
    gsl_matrix_free(result);

    return avg_color;
}