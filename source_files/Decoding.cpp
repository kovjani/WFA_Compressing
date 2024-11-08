#include "../header_files/Decoding.h"

#include "../header_files/Element.h"

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

    char line[16384];
    // std::vector<char> line;
    char *end;

    // The first line is the size of the square matrices
    fgets(line, sizeof(line), wfa_file);
    this->n = atoi(line);

    fgets(line, sizeof(line), wfa_file);     // \n

    // Allocate memory
    this->Inn = new Element*[this->n];
    this->F = new Element*[this->n];

    this->A = new Element*[this->n];
    this->B = new Element*[this->n];
    this->C = new Element*[this->n];
    this->D = new Element*[this->n];

    // Initialize I
    this->I = new Element(1, 0);

    // Initialize the Inn, it's an nxn identity matrix for the first call.
    for (int i = 0; i < this->n; ++i) {
        this->Inn[i] = new Element(1, i);
    }

    // Initialize F
    // The second line is the F (n*1 matrix), the average colors of states.
    {
        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        for (int i = 0; i < this->n; ++i) {
            this->F[i] = new Element(std::strtod(token, &end), 0);
            token = strtok(NULL, " "); // Get the next token
        }

        g_free(token);
    }
    fgets(line, sizeof(line), wfa_file);     // \n

    // Initialize matrix A
    for (int i = 0; i < this->n; ++i) {

        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        int j= std::strtod(token, &end);
        token = strtok(NULL, " "); // Get the next token

        double value = std::strtod(token, &end);
        token = strtok(NULL, " "); // Get the next token

        this->A[i] = new Element(value, j);

        g_free(token);
    }

    fgets(line, sizeof(line), wfa_file);     // \n

    // Initialize matrix B
    for (int i = 0; i < this->n; ++i) {

        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        int j = std::strtod(token, &end);
        token = strtok(NULL, " "); // Get the next token

        double value = std::strtod(token, &end);
        token = strtok(NULL, " "); // Get the next token

        this->B[i] = new Element(value, j);

        g_free(token);
    }

    fgets(line, sizeof(line), wfa_file);     // \n

    // Initialize matrix C
    for (int i = 0; i < this->n; ++i) {

        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        int j = std::strtod(token, &end);
        token = strtok(NULL, " "); // Get the next token

        double value = std::strtod(token, &end);
        token = strtok(NULL, " "); // Get the next token

        this->C[i] = new Element(value, j);

        g_free(token);
    }

    fgets(line, sizeof(line), wfa_file);     // \n

    // Initialize matrix D
    for (int i = 0; i < this->n; ++i) {

        fgets(line, sizeof(line), wfa_file);
        char *token = strtok(line, " "); // Split by space

        int j = std::strtod(token, &end);
        token = strtok(NULL, " "); // Get the next token

        double value = std::strtod(token, &end);
        token = strtok(NULL, " "); // Get the next token

        this->D[i] = new Element(value, j);

        g_free(token);
    }
}

Decoding::~Decoding() {
    // Free the allocated memory

    free_array_of_elements(this->A, this->n);
    free_array_of_elements(this->B, this->n);
    free_array_of_elements(this->C, this->n);
    free_array_of_elements(this->D, this->n);
    free_array_of_elements(this->Inn, this->n);
    free_array_of_elements(this->F, this->n);

    delete this->I;

    delete this->pixels_colors;
}

void Decoding::Start(char *directory, char *saved_filename) {
    DecodePixelsColors(this->depth, 0, 0, this->I, this->Inn);
    char *saved_file_path = SaveDecodedImage(directory, saved_filename);

    if(!this->testing) {
        OpenImage(saved_file_path);
    }

    g_free(saved_file_path);
}

void Decoding::DecodePixelsColors(int level, int x, int y, const Element* &previous_matrix, Element **next_matrix) {

    if(level > 0 ) {
        double value = 0;
        int res_j = 0;

        // Matrix multiplication
        // Previous_matrix always has one element, because the automata is deterministic.
        const Element *pre = previous_matrix;
        for (int i = 0; i < this->n; ++i) {
            Element *next = next_matrix[i];
            // i = next->i
            if(pre->j == i) {
                value = pre->value * next->value;
                // pre->i is always 0, because the previous matrix is a row vector
                res_j = next->j;
                break;
            }
        }

        const Element* result = new Element(value, res_j);
        int quadrant_size = 1 << level; // 2^level

        DecodePixelsColors(level-1, x, y, result, this->A);
        DecodePixelsColors(level-1, x + quadrant_size/2, y, result, this->B);
        DecodePixelsColors(level-1, x, y + quadrant_size/2, result, this->C);
        DecodePixelsColors(level-1, x + quadrant_size/2, y + quadrant_size/2,result, this->D);

        delete result;
    }
    // else
    // Pixels
    double average_color = 0;
    const Element *pre = previous_matrix;

    for (int i = 0; i < this->n; ++i) {
        // i = this->F[i]->i
        if(pre->j == i) {
            average_color = pre->value * this->F[i]->value;
            break;
        }
    }

    this->pixels_colors[x * this->decoding_image_size + y] = average_color;
}

void Decoding::free_array_of_elements(Element** &arr, int size) {
    for (int i = 0; i < size; ++i) {
        delete arr[i];
        arr[i] = nullptr;
    }
    delete[] arr;
}

void Decoding::free_vector_of_elements(std::vector<Element*> &vec) {
    for (int i = 0; i < vec.size(); ++i) {
        delete vec[i];
        vec[i] = nullptr;
    }
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