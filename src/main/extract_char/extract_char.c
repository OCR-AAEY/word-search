#include "extract_char.h"

void extarct(const Matrix *matrix, size_t x0, size_t y0, size_t x1, size_t y1)
{
    size_t width = 1 + x1 - x0;
    size_t height = 1 + y1 - y0;
    /* double **mat = malloc(sizeof(double *) * (height));

    for (size_t i = 0; i < height; i++)
    {
        mat[i] = malloc(sizeof(double) * (width));
    }
    for (size_t j = y0; j <= y1; j++)
    {
        for (size_t i = x0; i <= x1; i++)
        {
            mat[j - y0][i - x0] = mat_coef(matrix, j, i);
        }
    }*/
    GdkPixbuf *pixbuf =
        gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    for (size_t j = 0; j < height; j++)
    {
        guchar *p = pixels + j * rowstride;
        for (size_t i = 0; i < width; i++)
        {
            guchar value = (guchar)(mat_coef(matrix,j+y0,i+x0));
            p[3 * i + 0] = value;
            p[3 * i + 1] = value;
            p[3 * i + 2] = value;
        }
    }
    save_pixbuf_to_png(pixbuf, "result.png", NULL);

    /*for (size_t j = 0; j < height; j++)
        free(mat[j]);
    free(mat);*/
    g_object_unref(pixbuf);
}
