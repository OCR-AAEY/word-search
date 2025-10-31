#include "extract_char.h"

void save_image_region(const Matrix *matrix, const char *name, size_t x0,
                       size_t y0, size_t x1, size_t y1)
{

    // Matrix check
    if (matrix == NULL)
    {
        errx(EXIT_FAILURE, "The matrix is NULL");
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
            double val = mat_coef(matrix, j + y0, i + x0);

            if (val > 255 || val < 0)
            {
                errx(EXIT_FAILURE, "value out of bound");
            }

            guchar value = (guchar)round(val);
            p[3 * i + 0] = value;
            p[3 * i + 1] = value;
            p[3 * i + 2] = value;
        }
    }
    GError *error;
    save_pixbuf_to_png(pixbuf, (char *)name, &error);

    if (error)
        g_error_free(error);

    g_object_unref(pixbuf);
}
