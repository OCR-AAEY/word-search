#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"

#define DEG2RAD(deg) ((deg) * M_PI / 180.0)

/// @brief Orientation of a line or object.
typedef enum Orientation
{
    /// Vertical orientation.
    Vertical,

    /// Horizontal orientation.
    Horizontal
} Orientation;

/// @brief Type of morphological transformation.
typedef enum MorphTransform
{
    /// Erosion operation.
    Erosion,

    /// Dilation operation.
    Dilation,

    /// Opening operation (erosion followed by dilation).
    Opening,

    /// Closing operation (dilation followed by erosion).
    Closing
} MorphTransform;

/// @brief Represents a line in polar coordinates.
typedef struct Line
{
    /// The distance from the origin to the line.
    double r;

    /// The angle of the line in degrees.
    double theta;
} Line;

/// @brief Represents a Point in a Matrix.
typedef struct Point
{
    /// The coordinate on the height axis.
    size_t h;

    /// The coordinate on the width axis.
    size_t w;
} Point;

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

Line **hough_transform_lines(Matrix *src, float theta_precision, double delta_r,
                             double delta_theta, size_t *size_out);

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
void print_lines(Line **lines, size_t size /*, size_t offset*/);

#endif