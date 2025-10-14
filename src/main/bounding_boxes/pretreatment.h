#ifndef M_PI
#define M_PI	3.14159265358979323846	/* pi */
#endif

#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include "matrix.h"
#include "image_loading.h"

Matrix *gaussian_blur(Matrix *pixels, double sigma, size_t kernel_size);
Matrix *image_to_grayscale(ImageData *img);
ImageData *pixel_matrix_to_image(Matrix *matrix);
#endif