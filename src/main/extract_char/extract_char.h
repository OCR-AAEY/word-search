#ifndef EXTRACT_H
#define EXTRACT_H

#include "../image_loader/image_loading.h"
#include "../matrix/matrix.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

void extarct(const Matrix *matrix, size_t x0, size_t y0, size_t x1, size_t y1);

#endif