/*
#ifndef UNIT_TEST
#include "bounding_boxes/pretreatment.h"
#include "bounding_boxes/visualization.h"
#include "matrix/matrix.h"
#include "rotation.h"
#include <err.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdlib.h>

#define TARGET_WIDTH 800

typedef struct
{
    GtkEntry *path_entry;
    GtkEntry *angle_entry;
    GtkImage *original_image;
    GtkImage *rotated_image;
} AppWidgets;

ImageData *create_image_data_from_pixbuf(GdkPixbuf *pix)
{
    int width = gdk_pixbuf_get_width(pix);
    int height = gdk_pixbuf_get_height(pix);
    int rowstride = gdk_pixbuf_get_rowstride(pix);
    int n_channels = gdk_pixbuf_get_n_channels(pix);
    guchar *pixels = gdk_pixbuf_get_pixels(pix);

    if (n_channels < 3)
        errx(EXIT_FAILURE, "Pixbuf must have at least 3 channels (RGB)");

    ImageData *img = malloc(sizeof(ImageData));
    if (!img)
        errx(EXIT_FAILURE, "Failed to allocate ImageData");

    img->width = width;
    img->height = height;
    img->pixels = malloc(width * height * sizeof(Pixel));
    if (!img->pixels)
    {
        free(img);
        errx(EXIT_FAILURE, "Failed to allocate pixels");
    }

    for (int y = 0; y < height; y++)
    {
        guchar *row = pixels + y * rowstride;
        for (int x = 0; x < width; x++)
        {
            guchar *p = row + x * n_channels;
            Pixel *dest = &img->pixels[y * width + x];
            dest->r = p[0];
            dest->g = p[1];
            dest->b = p[2];
        }
    }

    return img;
}

static ImageData *resize_image(ImageData *img)
{
    if (!img || img->width <= 0 || img->height <= 0)
        return NULL;

    if (img->width <= TARGET_WIDTH)
        return load_image(
            "assets/sample_images/level_2_image_1.png"); // fallback (will be
                                                         // overwritten)

    double scale = (double)TARGET_WIDTH / img->width;
    int new_w = TARGET_WIDTH;
    int new_h = (int)(img->height * scale);

    GdkPixbuf *pix = create_pixbuf_from_image_data(img);
    if (!pix)
        return NULL;

    GdkPixbuf *scaled =
        gdk_pixbuf_scale_simple(pix, new_w, new_h, GDK_INTERP_BILINEAR);
    g_object_unref(pix);

    if (!scaled)
        return NULL;

    ImageData *resized = create_image_data_from_pixbuf(scaled);
    g_object_unref(scaled);
    return resized;
}

static void on_rotate_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    AppWidgets *widgets = (AppWidgets *)user_data;
    if (!widgets)
        return;

    const char *path = gtk_entry_get_text(widgets->path_entry);
    const char *angle_text = gtk_entry_get_text(widgets->angle_entry);

    if (!path || strlen(path) == 0)
    {
        g_warning("No image path provided.");
        return;
    }

    double angle = atof(angle_text);
    g_message("Loading '%s' and rotating by %.2f degrees", path, angle);

    ImageData *img = load_image(path);
    if (!img)
    {
        g_warning("Failed to load image: %s", path);
        return;
    }

    ImageData *resized = resize_image(img);
    if (resized)
    {
        free_image(img);
        img = resized;
    }

    Matrix *m = image_to_grayscale(img);
    if (!m)
    {
        free_image(img);
        g_warning("Failed to convert image to grayscale.");
        return;
    }

    Matrix *rot = rotate_matrix(m, angle);
    if (!rot)
    {
        free_image(img);
        g_warning("Rotation failed.");
        return;
    }

    ImageData *img_end = pixel_matrix_to_image(rot);
    if (!img_end)
    {
        free_image(img);
        g_warning("Failed to convert rotated matrix to image.");
        return;
    }

    GdkPixbuf *pix_orig = create_pixbuf_from_image_data(img);
    GdkPixbuf *pix_rot = create_pixbuf_from_image_data(img_end);

    gtk_image_set_from_pixbuf(GTK_IMAGE(widgets->original_image), pix_orig);
    gtk_image_set_from_pixbuf(GTK_IMAGE(widgets->rotated_image), pix_rot);

    g_object_unref(pix_orig);
    g_object_unref(pix_rot);
    free_image(img);
    free_image(img_end);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    (void)user_data;
    AppWidgets *widgets = g_new0(AppWidgets, 1);

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Rotation Demo (with resize)");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 600);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), input_box, FALSE, FALSE, 0);

    GtkWidget *path_label = gtk_label_new("Image Path:");
    widgets->path_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(widgets->path_entry,
                                   "e.g. assets/sample_images/pic.png");

    GtkWidget *angle_label = gtk_label_new("Angle:");
    widgets->angle_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(widgets->angle_entry, "e.g. 25");

    GtkWidget *rotate_button = gtk_button_new_with_label("Rotate");

    gtk_box_pack_start(GTK_BOX(input_box), path_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), GTK_WIDGET(widgets->path_entry),
                       TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), angle_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), GTK_WIDGET(widgets->angle_entry),
                       FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), rotate_button, FALSE, FALSE, 0);

    GtkWidget *images_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), images_box, TRUE, TRUE, 0);

    widgets->original_image = GTK_IMAGE(gtk_image_new());
    widgets->rotated_image = GTK_IMAGE(gtk_image_new());

    gtk_box_pack_start(GTK_BOX(images_box), GTK_WIDGET(widgets->original_image),
                       TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(images_box), GTK_WIDGET(widgets->rotated_image),
                       TRUE, TRUE, 0);

    g_signal_connect(rotate_button, "clicked", G_CALLBACK(on_rotate_clicked),
                     widgets);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new("com.example.rotationtool",
                                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

#endif
*/
