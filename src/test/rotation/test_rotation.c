#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <err.h>
#include <stdint.h>
#include "image_loader/image_loading.h"
#include "rotation-manual/rotation.h"

int main(int argc, char *argv[])
{
    if (argc != 4) {
        g_print("Usage: %s <input.png> <output.png> <degrees>\n", argv[0]);
        return 1;
    }

    gtk_init(&argc, &argv);

    ImageData *img = load_image(argv[1]);
    if (!img) {
        g_printerr("Failed to load image '%s'\n", argv[1]);
        return 1;
    }

    ImageData *rotated = rotate_image(img,  atof(argv[3]));
    if (!rotated) {
        g_printerr("Rotation failed\n");
        free_image(img);
        return 1;
    }

    GdkPixbuf *pixbuf_out = create_pixbuf_from_image_data(rotated);
    if (!pixbuf_out) {
        g_printerr("Failed to create GdkPixbuf from rotated image\n");
        free_image(rotated);
        free_image(img);
        return 1;
    }

    GError *error = NULL;
    if (!save_pixbuf_to_png(pixbuf_out, argv[2], &error)) {
        if (error) {
            g_printerr("Error saving image: %s\n", error->message);
            g_error_free(error);
        }
        g_object_unref(pixbuf_out);
        free_image(rotated);
        free_image(img);
        return 1;
    }

    g_print("✅ Rotated image saved to %s\n", argv[2]);

    g_object_unref(pixbuf_out);
    free_image(rotated);
    free_image(img);

    return 0;
}
