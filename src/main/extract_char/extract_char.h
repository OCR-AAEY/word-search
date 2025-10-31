#ifndef EXTRACT_H
#define EXTRACT_H

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include <err.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>

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

void save_image_region(const Matrix *matrix, const char *name, size_t x0,
                       size_t y0, size_t x1, size_t y1);

#endif
