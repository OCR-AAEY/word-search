#include "pretreatment.h"
#include <err.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>
#include <stdlib.h>

/// @brief Converts a Pixel to grayscale using Rec.709 luminance weights.
///
/// This function applies the weighted sum formula:
/// Gray = 0.2126 * R + 0.7152 * G + 0.0722 * B
/// These coefficients correspond to the Rec.709 standard, which models
/// human perception by giving more weight to green and less to blue.
///
/// @param[in] pixel Pointer to the input RGB pixel.
/// @return Grayscale value in the range [0, 255].
uint8_t pixel_to_grayscale(Pixel *pixel)
{
    return (uint8_t)round(0.2126 * pixel->r + 0.7152 * pixel->g +
                          0.0722 * pixel->b);
}

/// @brief Converts an ImageData to a grayscale matrix.
///
/// Each pixel is converted to grayscale using @ref pixel_to_grayscale,
/// and stored in a matrix. The result is a single-channel image
/// suitable for further image processing (e.g., Gaussian blur, edge detection).
///
/// @param[in] img Pointer to the input image data (RGB).
/// @return A newly allocated matrix containing grayscale values.
///         Each element is in the range [0, 255].
/// @see pixel_to_grayscale
Matrix *image_to_grayscale(ImageData *img)
{
    Matrix *grayscaled_pixels = mat_create_empty(img->height, img->width);
    for (size_t h = 0; h < img->height; h++)
    {
        for (size_t w = 0; w < img->width; w++)
        {
            double *gray_scaled_pixel = mat_coef_addr(grayscaled_pixels, h, w);
            *gray_scaled_pixel = pixel_to_grayscale(get_pixel(img, h, w));
        }
    }
    return grayscaled_pixels;
}

/// @brief Converts a grayscale matrix to an RGB image.
/// @param[in] matrix Pointer to the input grayscale matrix (values 0.0–255.0).
/// @return Pointer to a newly allocated ImageData containing RGB pixels.
/// @throw Throws if allocations failed or the values in the matrix are not
/// valid
ImageData *pixel_matrix_to_image(Matrix *matrix)
{
    ImageData *img = malloc(sizeof(ImageData));
    if (img == NULL)
        errx(EXIT_FAILURE, "Failed to allocate the ImageData struct");

    size_t height = mat_height(matrix);
    size_t width = mat_width(matrix);
    Pixel *pixels = malloc(height * width * sizeof(Pixel));

    if (pixels == NULL)
    {
        free_image(img);
        errx(EXIT_FAILURE, "Failed to allocate the pixels array");
    }

    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double gray_scaled_pixel = mat_coef(matrix, h, w);

            if (gray_scaled_pixel < 0 || gray_scaled_pixel > 255)
            {
                free_image(img);
                free(pixels);
                errx(EXIT_FAILURE, "Matrix values must be between 0 and 255");
            }

            Pixel *pixel = &pixels[h * width + w];
            // Convert the grayscale double to a uint8_t (with rounding)
            uint8_t gray = (uint8_t)floor(gray_scaled_pixel + 0.5);
            pixel->r = gray;
            pixel->g = gray;
            pixel->b = gray;
        }
    }
    img->height = height;
    img->width = width;
    img->pixels = pixels;
    return img;
}

/// @brief Computes the value of a 1D Gaussian function at (x).
///
/// @param[in] x The x-coordinate relative to the center of the kernel vector.
/// @param[in] sigma The standard deviation of the Gaussian.
/// @return The Gaussian value at the given coordinates.
double gaussian_function(int x, double sigma)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

/// @brief Return the 1D Gaussian kernel for a given sigma and size
///
/// The kernel is centered, normalized and its values are computed
/// using the 1D Gaussian function.
///
/// @param[in] sigma The standard deviation of the Gaussian.
/// @param[in] kernel_size The size of the kernel. Must be odd; even sizes
/// return NULL.
/// @return The gaussian kernel vector or NULL if kernel_size is incorrect
/// (even).
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

/// @brief Clamp an integer value between a minimum and a maximum.
/// @param[in] value The integer value to clamp.
/// @param[in] min The minimum allowed value.
/// @param[in] max The maximum allowed value.
/// @return The clamped integer in the range [min, max].
/// @throw Throws if the minimum is strictly greater than the maximum
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

/// @brief Performs a 1D horizontal convolution on an image.
///
/// Each pixel in the output image is computed as a weighted sum of its
/// horizontal neighbors in the input image, using the specified 1D convolution
/// kernel. Out-of-bounds pixels are handled by clamping to the nearest valid
/// pixel (edge repetition).
///
/// @param[in] src Pointer to the source image matrix.
/// @param[in] kernel Pointer to a 1D array of convolution weights.
/// @param[in] kernel_size Length of the 1D kernel (must be odd).
/// @return Pointer to the matrix containing the horizontally
///         convolved image.
/// @throw Throws if the @p src is NULL or kernel_size is even.
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
            double *dst_pixel = mat_coef_addr(dst, x, y);
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

/// @brief Performs a 1D vertical convolution on an image.
///
/// Each pixel in the output image is computed as a weighted sum of its
/// vertical neighbors in the input image, using the specified 1D convolution
/// kernel. Out-of-bounds pixels are handled by clamping to the nearest valid
/// pixel (edge repetition).
///
/// @param[in] src Pointer to the source image matrix.
/// @param[in] kernel Pointer to a 1D array of convolution weights.
/// @param[in] kernel_size Length of the 1D kernel (must be odd).
/// @return Pointer to the matrix containing the vertically
///         convolved image.
/// @throw Throws if the @p src is NULL or kernel_size is even.
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
            double *dst_pixel = mat_coef_addr(dst, x, y);
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

/// @brief Applies a Gaussian blur to an input image using separable
/// convolution.
///
/// This function creates a normalized 1D Gaussian kernel based on the specified
/// standard deviation and kernel size, and applies it in two passes:
///  - A horizontal 1D convolution
///  - A vertical 1D convolution
/// This separated approach improves performances.
///
/// @param[in] src Pointer to the input image matrix to be blurred.
/// @param[in] sigma The standard deviation of the Gaussian blur.
/// @param[in] kernel_size Size of the square Gaussian kernel (must be odd).
/// @return Pointer to a newly allocated matrix containing the blurred image.
/// @throw Throws if the kernel_size is even or the @p src is NULL.
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

/// @brief Applies adaptive Gaussian thresholding to an input image.
///
/// Each pixel is compared to a local threshold computed from the
/// Gaussian-blurred version of the image. The local threshold for a pixel is
/// defined as the Gaussian-weighted mean of its neighborhood minus a constant
/// @p c.
///
/// The output pixel value is set to @p max_value if the original pixel is
/// greater than its local threshold, or 0 otherwise.
///
/// This implementation uses a separable Gaussian blur for efficiency.
///
/// @param[in] src Pointer to the input grayscale image matrix.
/// @param[in] max_value Value assigned to pixels that pass the threshold.
/// @param[in] kernel_size Size of the Gaussian kernel (must be odd).
/// @param[in] sigma Standard deviation of the Gaussian kernel (must be > 0).
/// @param[in] c Constant subtracted from the local mean to determine the
/// threshold.
/// @return Pointer to a newly allocated matrix containing the thresholded
/// image.
/// @throw Throws if the @p max_value or @p sigma are invalid or if @p src is
/// NULL.
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
    size_t height = mat_height(src);
    size_t width = mat_width(src);
    Matrix *dest = mat_create_empty(height, width);

    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double *pixel = mat_coef_addr(dest, h, w);
            double blurred_pixel = mat_coef(blurred, h, w);
            double src_pixel = mat_coef(src, h, w);
            double T = blurred_pixel - c;
            *pixel = src_pixel > T ? max_value : 0;
        }
    }
    mat_free(blurred);
    return dest;
}

/// @brief Performs a 1D morphological transformation (erosion or dilation)
/// along a specified orientation.
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] kernel_size Size of the kernel. OpenCV-style anchor is used; even
/// sizes are supported.
/// @param[in] transform Type of morphological transformation (Erosion or
/// Dilation).
/// @param[in] orientation Orientation of the 1D pass (Horizontal or Vertical).
/// @return Pointer to a newly allocated matrix containing the transformed
/// image.
/// @throw Throws if src is NULL, transform is invalid, or orientation is
/// invalid.
/// @note Caller is responsible for freeing the returned matrix using
/// mat_free().
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
            double *dst_pixel = mat_coef_addr(dst, x, y);
            double extreme_val = transform == Erosion ? 255 : 0;
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
                case Erosion:
                    if (image_pixel < extreme_val)
                        extreme_val = image_pixel;
                    break;

                case Dilation:
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

/// @brief Applies erosion to a matrix using a separable 2-pass approach
/// (horizontal then vertical).
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] kernel_size Size of the kernel. Even kernel sizes are supported.
/// @return Pointer to a newly allocated matrix containing the eroded image.
/// @throw Throws if src is NULL.
/// @note Caller is responsible for freeing the returned matrix using
/// mat_free().
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

/// @brief Applies dilation to a matrix using a separable 2-pass approach
/// (horizontal then vertical).
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] kernel_size Size of the kernel. Even kernel sizes are supported.
/// @return Pointer to a newly allocated matrix containing the dilated image.
/// @throw Throws if src is NULL.
/// @note Caller is responsible for freeing the returned matrix using
/// mat_free().
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

/// @brief Applies a morphological transformation to a matrix (Erosion,
/// Dilation, Opening, Closing).
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] kernel_size Size of the kernel. Even kernel sizes are supported.
/// @param[in] transform Type of morphological transformation (Erosion,
/// Dilation, Opening, Closing).
/// @return Pointer to a newly allocated matrix containing the transformed
/// image.
/// @throw Throws if src is NULL or transform type is invalid.
/// @note Caller is responsible for freeing the returned matrix using
/// mat_free().
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

#ifndef UNIT_TEST

int main()
{
    // ImageData *img = load_image("assets/test_images/montgolfiere.jpg");
    // ImageData *img = load_image("assets/sample_images/level_1_image_1.png");
    // ImageData *img = load_image("assets/sample_images/level_1_image_2.png");
    ImageData *img = load_image("assets/sample_images/level_2_image_1.png");
    // ImageData *img = load_image("assets/sample_images/level_2_image_2.png");

    Matrix *gray = image_to_grayscale(img);
    // ImageData *gray_img = pixel_matrix_to_image(gray);
    // GdkPixbuf *pixbuf_gray = create_pixbuf_from_image_data(gray_img);
    // save_pixbuf_to_png(pixbuf_gray, "gray.png", NULL);
    // g_object_unref(pixbuf_gray);
    // free_image(gray_img);

    // Matrix *blured = gaussian_blur(gray, 10, 11);
    //  ImageData *blured_img = pixel_matrix_to_image(blured);
    //  GdkPixbuf *pixbuf_blur = create_pixbuf_from_image_data(blured_img);
    //  save_pixbuf_to_png(pixbuf_blur, "blured.png", NULL);
    //  g_object_unref(pixbuf_blur);
    //  free_image(blured_img);

    Matrix *threshold = adaptative_gaussian_thresholding(gray, 255, 11, 10, 7);
    mat_free(gray);

    Matrix *opening = morph_transform(threshold, 2, Opening);
    mat_free(threshold);

    Matrix *closing = morph_transform(opening, 2, Closing);
    mat_free(opening);

    ImageData *result_img = pixel_matrix_to_image(closing);
    GdkPixbuf *pixbuf_result = create_pixbuf_from_image_data(result_img);
    save_pixbuf_to_png(pixbuf_result, "result.png", NULL);
    g_object_unref(pixbuf_result);
    free_image(result_img);

    free_image(img);

    // mat_free(blured);
    mat_free(closing);

    return EXIT_SUCCESS;
}

#endif