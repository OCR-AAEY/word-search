#ifndef M_PI
#define M_PI	3.14159265358979323846	/* pi */
#endif

#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include "matrix/matrix.h"
#include "image_loader/image_loading.h"

Matrix *gaussian_blur(Matrix *pixels, double sigma, size_t kernel_size);
Matrix *image_to_grayscale(ImageData *img);
ImageData *pixel_matrix_to_image(Matrix *matrix);

/// ============= internal functions ===============

uint8_t pixel_to_grayscale(Pixel *pixel);
double gaussian_function(int x, int y, double sigma);
Matrix *gaussian_kernel(double sigma, size_t kernel_size);
double sum_matrix_coefs(Matrix *mat);
Matrix *scalar_multiplication(Matrix *mat, double a);
Matrix *gaussian_normalize(Matrix *g);
int clamp(int value, int min, int max);
Matrix *gaussian_convolute(Matrix *g, Matrix *image, size_t kernel_size);

#endif