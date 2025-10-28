#ifndef EXTRACT_H
#define EXTRACT_H

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include <err.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>
void save_image_region(const Matrix *matrix, char *name, size_t x0, size_t y0,
                       size_t x1, size_t y1);

#endif
