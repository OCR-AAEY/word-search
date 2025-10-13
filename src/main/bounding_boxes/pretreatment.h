#ifndef M_PI
#define M_PI	3.14159265358979323846	/* pi */
#endif

#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include "matrix/matrix.h"

Matrix *gaussian_blur(Matrix *pixels, double sigma, size_t kernel_size);

#endif