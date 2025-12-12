#ifndef EXTRACT_H
#define EXTRACT_H

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include <err.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>

/// @brief Saves a rectangular region of a matrix as a PNG image.
/// Extracts the region defined by (x0, y0) to (x1, y1), normalizes coordinate
/// order if needed, and writes the cropped area to the given filename.
/// @param[in] matrix Pointer to the source matrix. Must not be NULL.
/// @param[in] name Output PNG filename. Must not be NULL.
/// @param[in] x0 Starting x-coordinate of the region.
/// @param[in] y0 Starting y-coordinate of the region.
/// @param[in] x1 Ending x-coordinate of the region.
/// @param[in] y1 Ending y-coordinate of the region.
/// @return 0 on success, or a negative error code:
///         -1 if matrix is NULL,
///         -2 if a pixel value is out of bounds,
///         -3 if saving the PNG fails.
int save_image_region(const Matrix *matrix, const char *name, size_t x0,
                      size_t y0, size_t x1, size_t y1);

#endif
