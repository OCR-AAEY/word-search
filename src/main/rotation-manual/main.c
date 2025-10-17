#include <gtk/gtk.h>
#include "rotation.h"

typedef struct {
    GtkImage *image_widget;
    GdkPixbuf *original;
} AppData;

static void on_slider_value_changed(GtkRange *range, gpointer user_data)
{
    AppData *data = user_data;
    double angle = gtk_range_get_value(range);

    // Rotate from original every time
    GdkPixbuf *rotated = rotate_pixbuf(data->original, angle);
    gtk_image_set_from_pixbuf(data->image_widget, rotated);
    g_object_unref(rotated);
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    // === Load original image ===
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("assets/sample_images/level_1_image_1.png", &error);
    if (!pixbuf) {
        g_printerr("Error loading image: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // === GTK UI ===
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Manual Rotation");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);

    GtkWidget *slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -90, 90, 1);
    gtk_scale_set_value_pos(GTK_SCALE(slider), GTK_POS_BOTTOM);
    gtk_box_pack_start(GTK_BOX(box), slider, FALSE, FALSE, 0);

    // === Connect events ===
    AppData data = {GTK_IMAGE(image), pixbuf};
    g_signal_connect(slider, "value-changed", G_CALLBACK(on_slider_value_changed), &data);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    g_object_unref(pixbuf);
    return 0;
}
