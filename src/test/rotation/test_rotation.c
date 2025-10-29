#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bounding_boxes/pretreatment.h"
#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "rotation-manual/rotation.h"


int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <input.png> <output.png> <angle>\n", argv[0]);
        return EXIT_FAILURE;
    }

    gtk_init(&argc, &argv);

    ImageData *img = load_image(argv[1]);
    if (img == NULL)
    {
        fprintf(stderr, "Failed to load image: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    Matrix *m = image_to_grayscale(img);
    if (m == NULL)
    {
        fprintf(stderr, "Failed to convert to matrix\n");
        free_image(img);
        return EXIT_FAILURE;
    }

    Matrix *rot = rotate_matrix(m, strtod(argv[3], NULL));
    if (rot == NULL)
    {
        fprintf(stderr, "Rotation failed\n");
        mat_free(m);
        free_image(img);
        return EXIT_FAILURE;
    }

    ImageData *rot_img = pixel_matrix_to_image(rot);
    if (rot_img == NULL)
    {
        fprintf(stderr, "Failed to convert rotated matrix to image\n");
        mat_free(m);
        mat_free(rot);
        free_image(img);
        return EXIT_FAILURE;
    }

    // Create pixbuf manually from ImageData pixels
    GdkPixbuf *pixbuf = gdk_pixbuf_new(
        GDK_COLORSPACE_RGB,
        FALSE,      // no alpha
        8,          // 8 bits per channel
        rot_img->width,
        rot_img->height
    );

    guchar *dest_pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    for (size_t y = 0; y < rot_img->height; y++)
    {
        for (size_t x = 0; x < rot_img->width; x++)
        {
            Pixel *p = &rot_img->pixels[y * rot_img->width + x];
            dest_pixels[y * rowstride + x * 3 + 0] = p->r;
            dest_pixels[y * rowstride + x * 3 + 1] = p->g;
            dest_pixels[y * rowstride + x * 3 + 2] = p->b;
        }
    }

    GError *error = NULL;
    if (!gdk_pixbuf_save(pixbuf, argv[2], "png", &error, NULL))
    {
        fprintf(stderr, "Failed to save image: %s\n", error->message);
        g_error_free(error);
    }

    mat_free(m);
    mat_free(rot);
    free_image(img);
    free_image(rot_img);
    g_object_unref(pixbuf);

    printf("Saved rotated image to %s\n", argv[2]);
    return EXIT_SUCCESS;
}
