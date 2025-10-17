#include <gtk/gtk.h>
#include "rotation.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        g_print("Usage: %s <input.png> <output.png>\n", argv[0]);
        return 1;
    }

    gtk_init(&argc, &argv);

    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(argv[1], &error);
    if (!pixbuf) {
        g_printerr("Error loading image: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    GdkPixbuf *rotated = rotate_pixbuf(pixbuf, 45.0); // rotate 45 degrees
    gdk_pixbuf_save(rotated, argv[2], "png", &error, NULL);
    if (error) {
        g_printerr("Error saving image: %s\n", error->message);
        g_error_free(error);
    }

    g_object_unref(pixbuf);
    g_object_unref(rotated);
    return 0;
}
