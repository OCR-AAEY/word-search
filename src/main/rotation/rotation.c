#include "rotation.h"
#include <err.h>
#include <math.h>
#include <stdlib.h>

Matrix *rotate_matrix(const Matrix *src, double angle)
{
    if (src == NULL)
    {
        errx(EXIT_FAILURE, "rotate_matrix: src matrix is NULL");
    }

    size_t w = (size_t)mat_width(src);
    size_t h = (size_t)mat_height(src);

    double rad = angle * M_PI / 180.0;

    size_t nw =
        (size_t)(fabs((double)w * cos(rad)) + fabs((double)h * sin(rad)) + 0.5);
    size_t nh =
        (size_t)(fabs((double)h * cos(rad)) + fabs((double)w * sin(rad)) + 0.5);

    Matrix *rotated = mat_create_empty(nh, nw);

    for (size_t y = 0; y < nh; y++)
    {
        for (size_t x = 0; x < nw; x++)
        {
            *mat_coef_addr(rotated, y, x) = 255.0;
        }
    }

    double cx = w / 2.0;
    double cy = h / 2.0;
    double ncx = nw / 2.0;
    double ncy = nh / 2.0;

    for (size_t y = 0; y < nh; y++)
    {
        for (size_t x = 0; x < nw; x++)
        {
            double tx = (x - ncx) * cos(rad) - (y - ncy) * sin(rad) + cx;
            double ty = (x - ncx) * sin(rad) + (y - ncy) * cos(rad) + cy;

            if (tx >= 0 && tx < w && ty >= 0 && ty < h)
            {
                int ix = (int)tx;
                int iy = (int)ty;
                double val = mat_coef(src, iy, ix);
                *mat_coef_addr(rotated, y, x) = val;
            }
        }
    }

    return rotated;
}

ImageData *rotate_image(ImageData *img, double angle)
{
    int w = img->width;
    int h = img->height;
    Pixel *p = img->pixels;

    double rad = angle * M_PI / 180.0;

    int nw = (int)(fabs(w * cos(rad)) + fabs(h * sin(rad)) + 0.5);
    int nh = (int)(fabs(h * cos(rad)) + fabs(w * sin(rad)) + 0.5);

    Pixel *np = calloc(nw * nh, sizeof(Pixel));
    if (!np)
    {
        errx(EXIT_FAILURE, "rotate_image malloc fail");
    }

    for (int i = 0; i < nw * nh; i++)
    {
        np[i].r = 255;
        np[i].g = 255;
        np[i].b = 255;
    }

    double cx = w / 2.0;
    double cy = h / 2.0;
    double ncx = nw / 2.0;
    double ncy = nh / 2.0;

    for (int y = 0; y < nh; y++)
    {
        for (int x = 0; x < nw; x++)
        {
            double tx = (x - ncx) * cos(rad) - (y - ncy) * sin(rad) + cx;
            double ty = (x - ncx) * sin(rad) + (y - ncy) * cos(rad) + cy;

            if (tx >= 0 && tx < w && ty >= 0 && ty < h)
            {
                int ix = (int)tx;
                int iy = (int)ty;
                np[y * nw + x] = p[iy * w + ix];
            }
        }
    }

    ImageData *out = malloc(sizeof(ImageData));
    if (!out)
    {
        errx(EXIT_FAILURE, "rotate_image malloc fail out");
    }

    out->width = nw;
    out->height = nh;
    out->pixels = np;

    return out;
}

#ifndef UNIT_TEST

#include "bounding_boxes/pretreatment.h"
#include <gtk/gtk.h>

typedef struct
{
    GtkEntry *path_entry;
    GtkEntry *angle_entry;
    GtkImage *original_image;
    GtkImage *rotated_image;
} AppWidgets;

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
        g_warning("No image path provided");
        return;
    }

    double angle = atof(angle_text);
    if (angle == 0.0)
        g_message("Rotating by 0 degrees (no change)");

    ImageData *img = load_image(path);
    if (!img)
    {
        g_warning("Failed to load image: %s", path);
        return;
    }

    Matrix *m = image_to_grayscale(img);
    if (!m)
    {
        g_warning("Failed to convert image to grayscale");
        free_image(img);
        return;
    }

    Matrix *rot = rotate_matrix(m, angle);
    if (!rot)
    {
        g_warning("Failed to rotate matrix");
        free_image(img);
        free_image(m);
        return;
    }

    ImageData *img_end = pixel_matrix_to_image(rot);
    if (!img_end)
    {
        g_warning("Failed to convert rotated matrix to image");
        free_image(img);
        free_image(m);
        free_image(rot);
        return;
    }

    GdkPixbuf *pix_orig = create_pixbuf_from_image_data(img);
    GdkPixbuf *pix_rot = create_pixbuf_from_image_data(img_end);

    gtk_image_set_from_pixbuf(GTK_IMAGE(widgets->original_image), pix_orig);
    gtk_image_set_from_pixbuf(GTK_IMAGE(widgets->rotated_image), pix_rot);

    g_object_unref(pix_orig);
    g_object_unref(pix_rot);
    free_image(img);
    free_image(m);
    free_image(rot);
    free_image(img_end);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    (void)user_data;

    AppWidgets *widgets = g_new0(AppWidgets, 1);

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Image Rotation Tool");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), input_box, FALSE, FALSE, 0);

    GtkWidget *path_label = gtk_label_new("Image Path:");
    widgets->path_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(widgets->path_entry),
        "ex: assets/sample_images/level_2_image_1.png");

    GtkWidget *angle_label = gtk_label_new("Angle:");
    widgets->angle_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->angle_entry), "ex: 45");

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
                                              G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

#endif
