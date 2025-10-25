#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"

enum Orientation
{
    Vertical,
    Horizontal
};

enum MorphTransform
{
    Erosion,
    Dilation,
    Opening,
    Closing
};

Matrix *gaussian_blur(const Matrix *src, double sigma, size_t kernel_size);

Matrix *image_to_grayscale(ImageData *img);

ImageData *pixel_matrix_to_image(Matrix *matrix);

Matrix *adaptative_gaussian_thresholding(const Matrix *src, double max_value,
                                         size_t kernel_size, double sigma,
                                         double c);

Matrix *erosion(const Matrix *src, size_t kernel_size);

Matrix *dilation(const Matrix *src, size_t kernel_size);

Matrix *morph_transform(Matrix *src, size_t kernel_size,
                        enum MorphTransform transform);

/// ============= internal functions ===============

uint8_t pixel_to_grayscale(Pixel *pixel);
double gaussian_function(int x, double sigma);
double *gaussian_kernel_1d(double sigma, size_t kernel_size);
Matrix *convolve_horizontally(const Matrix *src, const double *kernel,
                              size_t kernel_size);
Matrix *convolve_vertically(const Matrix *src, const double *kernel,
                            size_t kernel_size);
int clamp(int value, int min, int max);
Matrix *morph_transformation_1d(const Matrix *src, size_t kernel_size,
                                enum MorphTransform transform,
                                enum Orientation orientation);
#endif