#include "pretreatment.h"
#include "bounding_boxes/visualization.h"
#include "utils/utils.h"
#include <err.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>
#include <stdlib.h>

uint8_t pixel_to_grayscale(Pixel *pixel)
{
    return (uint8_t)round(0.2126 * pixel->r + 0.7152 * pixel->g +
                          0.0722 * pixel->b);
}

Matrix *image_to_grayscale(ImageData *img)
{
    Matrix *grayscaled_pixels = mat_create_empty(img->height, img->width);
    for (size_t h = 0; h < img->height; h++)
    {
        for (size_t w = 0; w < img->width; w++)
        {
            double *gray_scaled_pixel = mat_coef_ptr(grayscaled_pixels, h, w);
            *gray_scaled_pixel = pixel_to_grayscale(get_pixel(img, h, w));
        }
    }
    return grayscaled_pixels;
}

double gaussian_function(int x, double sigma)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

double *gaussian_kernel_1d(double sigma, size_t kernel_size)
{
    int m = (kernel_size - 1) / 2;
    double *kernel = malloc(kernel_size * sizeof(double));
    double sum = 0.0;

    for (int i = -m; i <= m; i++)
    {
        double value = gaussian_function(i, sigma);
        kernel[i + m] = value;
        sum += value;
    }

    // Normalize to make sum = 1
    for (size_t i = 0; i < kernel_size; i++)
        kernel[i] /= sum;

    return kernel;
}

int clamp(int value, int min, int max)
{
    if (max < min)
        errx(EXIT_FAILURE, "Minimum must be less than or equal maximum");
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

Matrix *convolve_horizontally(const Matrix *src, const double *kernel,
                              size_t kernel_size)
{
    if (kernel_size % 2 == 0)
        errx(EXIT_FAILURE, "The kernel size must be an odd number");

    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    int m = (kernel_size - 1) / 2;
    size_t height = mat_height(src);
    size_t width = mat_width(src);
    Matrix *dst = mat_create_empty(height, width);

    for (size_t x = 0; x < height; x++)
        for (size_t y = 0; y < width; y++)
        {
            double *dst_pixel = mat_coef_ptr(dst, x, y);
            for (int i = -m; i <= m; i++)
            {
                // clamp will treat indexes out of bound to stay at the last
                // acceptable value. This will repeat the pixel inside the
                // image to fill the missing pixels.
                double image_pixel =
                    mat_coef(src, x, clamp(y + i, 0, width - 1));
                // i is the index from the center of the kernel
                // by adding m we make sure that we use the indexing method
                // of the kernel vector
                double weight = kernel[m + i];
                *dst_pixel += weight * image_pixel;
            }
        }
    return dst;
}

Matrix *convolve_vertically(const Matrix *src, const double *kernel,
                            size_t kernel_size)
{
    if (kernel_size % 2 == 0)
        errx(EXIT_FAILURE, "The kernel size must be an odd number");

    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    int m = (kernel_size - 1) / 2;
    size_t height = mat_height(src);
    size_t width = mat_width(src);
    Matrix *dst = mat_create_empty(height, width);

    for (size_t x = 0; x < height; x++)
        for (size_t y = 0; y < width; y++)
        {
            double *dst_pixel = mat_coef_ptr(dst, x, y);
            for (int i = -m; i <= m; i++)
            {
                // clamp will treat indexes out of bound to stay at the last
                // acceptable value. This will repeat the pixel inside the
                // image to fill the missing pixels.
                double image_pixel =
                    mat_coef(src, clamp(x + i, 0, height - 1), y);
                // i is the index from the center of the kernel
                // by adding m we make sure that we use the indexing method
                // of the kernel vector
                double weight = kernel[m + i];
                *dst_pixel += weight * image_pixel;
            }
        }
    return dst;
}

Matrix *gaussian_blur(const Matrix *src, double sigma, size_t kernel_size)
{
    if (kernel_size % 2 == 0)
        errx(EXIT_FAILURE, "The kernel size must be an odd number");

    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    const double *kernel = gaussian_kernel_1d(sigma, kernel_size);

    Matrix *tmp = convolve_horizontally(src, kernel, kernel_size);
    Matrix *blurred = convolve_vertically(tmp, kernel, kernel_size);

    mat_free(tmp);
    free((void *)kernel);
    return blurred;
}

Matrix *adaptative_gaussian_thresholding(const Matrix *src, double max_value,
                                         size_t kernel_size, double sigma,
                                         double c)
{
    if (max_value < 0)
        errx(EXIT_FAILURE, "The max value must be a positive double");
    if (sigma <= 0)
        errx(EXIT_FAILURE, "Sigma must be positive");
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    Matrix *blurred = gaussian_blur(src, sigma, kernel_size);
    export_matrix(blurred, GAUSSIAN_BLURRED_FILENAME);
    size_t height = mat_height(src);
    size_t width = mat_width(src);
    Matrix *dest = mat_create_empty(height, width);

    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double *pixel = mat_coef_ptr(dest, h, w);
            double blurred_pixel = mat_coef(blurred, h, w);
            double src_pixel = mat_coef(src, h, w);
            double T = blurred_pixel - c;
            *pixel = src_pixel > T ? max_value : 0;
        }
    }
    mat_free(blurred);
    return dest;
}

Matrix *morph_transformation_1d(const Matrix *src, size_t kernel_size,
                                enum MorphTransform transform,
                                enum Orientation orientation)
{

    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    if (transform != Erosion && transform != Dilation)
        errx(EXIT_FAILURE, "Invalid MorphTransform type");

    if (orientation != Vertical && orientation != Horizontal)
        errx(EXIT_FAILURE, "Invalid Orientation");

    int anchor = kernel_size / 2; // handles even kernels
    size_t height = mat_height(src);
    size_t width = mat_width(src);
    Matrix *dst = mat_create_empty(height, width);

    for (size_t x = 0; x < height; x++)
        for (size_t y = 0; y < width; y++)
        {
            double *dst_pixel = mat_coef_ptr(dst, x, y);
            double extreme_val = transform == Erosion ? 0 : 255;
            for (int i = -anchor; i < (int)kernel_size - anchor; i++)
            {
                // clamp will treat indexes out of bound to stay at the last
                // acceptable value. This will repeat the pixel inside the
                // image to fill the missing pixels.
                double image_pixel;
                switch (orientation)
                {
                case Horizontal:
                    image_pixel = mat_coef(src, x, clamp(y + i, 0, width - 1));
                    break;

                case Vertical:
                    image_pixel = mat_coef(src, clamp(x + i, 0, height - 1), y);
                    break;
                }

                // i is the index from the center of the kernel
                // by adding m we make sure that we use the indexing method
                // of the kernel vector
                switch (transform)
                {
                case Dilation:
                    if (image_pixel < extreme_val)
                        extreme_val = image_pixel;
                    break;

                case Erosion:
                    if (image_pixel > extreme_val)
                        extreme_val = image_pixel;
                    break;
                default:
                    errx(EXIT_FAILURE, "Invalid MorphTransform type");
                }
            }
            *dst_pixel = extreme_val;
        }
    return dst;
}

Matrix *erosion(const Matrix *src, size_t kernel_size)
{

    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");
    Matrix *tmp =
        morph_transformation_1d(src, kernel_size, Erosion, Horizontal);
    Matrix *eroded =
        morph_transformation_1d(tmp, kernel_size, Erosion, Vertical);

    mat_free(tmp);
    return eroded;
}

Matrix *dilation(const Matrix *src, size_t kernel_size)
{

    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    Matrix *tmp =
        morph_transformation_1d(src, kernel_size, Dilation, Horizontal);
    Matrix *dilated =
        morph_transformation_1d(tmp, kernel_size, Dilation, Vertical);

    mat_free(tmp);
    
    return dilated;
}

Matrix *morph_transform(Matrix *src, size_t kernel_size,
                        enum MorphTransform transform)
{
    switch (transform)
    {
    case Erosion:
        return erosion(src, kernel_size);

    case Dilation:
        return dilation(src, kernel_size);

    case Opening:
        Matrix *eroded = erosion(src, kernel_size);
        Matrix *opened = dilation(eroded, kernel_size);
        mat_free(eroded);
        return opened;

    case Closing:
        Matrix *dilated = dilation(src, kernel_size);
        Matrix *closed = erosion(dilated, kernel_size);
        mat_free(dilated);
        return closed;

    default:
        errx(EXIT_FAILURE, "Invalid MorphTransform type");
    }
}
