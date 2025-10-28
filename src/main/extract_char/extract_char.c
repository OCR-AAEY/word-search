#include "extract_char.h"

void save_image_region(const Matrix *matrix, char *name, size_t x0, size_t y0,
                       size_t x1, size_t y1)
{
    /*
     @brief Saves a region of a matrix as an image in PNG format.

     This function extracts a rectangular region from a matrix and saves it as
     an image. The matrix is expected to contain grayscale values, which are
     mapped to RGB pixels.

     @param matrix The matrix containing pixel values . It must not be NULL.
     @param name The name of the file where the image will be saved (in PNG
     format).
     @param x0 The x-coordinate of the top-left corner of the region to extract.
     @param y0 The y-coordinate of the top-left corner of the region to extract.
     @param x1 The x-coordinate of the bottom-right corner of the region to
     extract.
     @param y1 The y-coordinate of the bottom-right corner of the region to
     extract.

     @return void

     @throws errx If the matrix is NULL or any pixel value is out of bounds
     (greater than 255 or less than 0).

     @note This function creates an image from the region defined by (x0, y0)
     and (x1, y1), where each pixel is mapped to a value(0 or 255) . The
     resulting image is saved in PNG format.

     @example
     // Example usage:
     save_image_region(matrix, "output.png", 10, 10, 50, 50);

     This would save a 40x40 region from the matrix starting at coordinates (10,
     10) to (50, 50) as an image named "output.png".
    */
    // Matrix check
    if (matrix == NULL)
    {
        errx(EXIT_FAILURE, "The matrix is NULL");
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
    save_pixbuf_to_png(pixbuf, name, NULL);

    g_object_unref(pixbuf);
}
