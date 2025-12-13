#include "extract_char.h"

int save_image_region(const Matrix *matrix, const char *name, size_t x0,
                      size_t y0, size_t x1, size_t y1)
{
    // Matrix check
    if (matrix == NULL)
    {
        fprintf(stderr, "save_image_region: The matrix is NULL\n");
        return -1;
    }

    size_t mat_h = mat_height(matrix);
    size_t mat_w = mat_width(matrix);
    if (x1 >= mat_w || y1 >= mat_h)
    {
        fprintf(stderr, "save_image_region: The region to save is invalid\n");
        return -2;
    }

    size_t tmp;
    if (x1 < x0)
    {
        tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    if (y1 < y0)
    {
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }
    size_t width = 1 + x1 - x0;
    size_t height = 1 + y1 - y0;

    GdkPixbuf *pixbuf =
        gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    for (size_t j = 0; j < height; j++)
    {
        guchar *p = pixels + j * rowstride;
        for (size_t i = 0; i < width; i++)
        {
            float val = mat_coef(matrix, j + y0, i + x0);

            if (val > 255 || val < 0)
            {
                fprintf(stderr, "save_image_region: value out of bound\n");
                return -3;
            }

            guchar value = (guchar)roundf(val);
            p[3 * i + 0] = value;
            p[3 * i + 1] = value;
            p[3 * i + 2] = value;
        }
    }
    int save_success = save_pixbuf_to_png(pixbuf, (char *)name);

    if (!save_success)
    {
        fprintf(stderr,
                "save_image_region: Failed to save the region as png\n");
        g_object_unref(pixbuf);
        return -4;
    }

    g_object_unref(pixbuf);
    return EXIT_SUCCESS;
}
