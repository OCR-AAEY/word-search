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

/// @brief Computes sine of an angle in degrees.
/// @param[in] degrees Angle in degrees.
/// @return Sine of the angle.
double sind(double degrees) { return sin(DEG2RAD(degrees)); }

/// @brief Computes cosine of an angle in degrees.
/// @param[in] degrees Angle in degrees.
/// @return Cosine of the angle.
double cosd(double degrees) { return cos(DEG2RAD(degrees)); }

/// @brief Creates an empty Hough accumulator matrix.
///
/// The accumulator has :
/// as columns : the angles theta (0 to 180 with a step of theta_precision) and
/// as rows : the distances r (from -r_max to r_max)
/// where the r_max is the diagonal of the image.
///
/// @param[in] height The height of the source image.
/// @param[in] width The width of the source image.
/// @param[in] theta_precision The angular precision in degrees. Must be
/// strictly positive.
/// @return A pointer to the allocated accumulator matrix.
/// @throw Throws if `theta_precision <= 0`.
Matrix *create_hough_accumulator(size_t height, size_t width,
                                 float theta_precision)
{
    if (theta_precision <= 0)
        errx(EXIT_FAILURE, "Theta precision must be strictly positive");

    // Maximum possible distance from the origin of the image (image diagonal)
    double diag = sqrt((double)height * height + (double)width * width);
    size_t r_max = (size_t)ceil(diag);

    // Number of rows = 2*r_max + 1 to include both negative and positive r
    // values
    size_t acc_height = 2 * r_max + 1;

    // Number of columns = number of theta steps (0°..180° exclusive)
    size_t acc_width = (size_t)round(180.0 / theta_precision);

    // columns are the theta values, and lines are the r values
    Matrix *accumulator = mat_create_empty(acc_height, acc_width);

    return accumulator;
}

/// @brief Populates a Hough accumulator from a source image.
/// For each pixel that is black (`0`), increment the r values corresponding to
/// all theta angles possible.
/// @param[in] src Pointer to the source binary image matrix. Must not be NULL.
/// @param[in,out] accumulator Pointer to the accumulator matrix to populate.
/// Must not be NULL.
/// @param[in] theta_precision The angular precision in degrees. Must be
/// strictly positive.
/// @param[out] max_count Pointer to the variable that will hold the maximum of
/// the accumulator. Must not be NULL.
/// @throw Throws if `src` or `accumulator` or `max_count` is NULL.
void populate_hough_lines(Matrix *src, Matrix *accumulator,
                          float theta_precision, size_t *max_count)
{
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    if (accumulator == NULL)
        errx(EXIT_FAILURE, "The accumulator matrix is NULL");

    if (max_count == NULL)
        errx(EXIT_FAILURE, "The max_count output parameter is NULL");

    size_t height = mat_height(src);
    size_t width = mat_width(src);
    size_t theta_max = mat_width(accumulator);
    // according to the definition of the accumulator height
    size_t r_max = (mat_height(accumulator) - 1) / 2;
    *max_count = 0;

    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double pixel = mat_coef(src, h, w);
            // if the pixel is not black, we skip it
            if (pixel != 0)
            {
                continue;
            }
            // for all the theta possible we calculate the r associated
            // this is derived from conversion between cartesian and polar
            // coordinates
            for (size_t theta_index = 0; theta_index < theta_max; theta_index++)
            {
                // theta_max is the maximum index for theta in the accumulator
                // since we have a step of theta_precision, we have :
                double r = w * cosd(theta_index * theta_precision) +
                           h * sind(theta_index * theta_precision);

                // since r can be at least -r_max, we shift it to an integer
                // to have the index in the accumulator
                size_t r_index = (size_t)round(r + r_max);

                double *accumulator_cell =
                    mat_coef_addr(accumulator, r_index, theta_index);
                (*accumulator_cell)++;

                // we keep track of the maximum value of the accumulator
                if (*accumulator_cell > *max_count)
                    *max_count = *accumulator_cell;
            }
        }
    }
}

/// @brief Extracts lines from a populated Hough accumulator.
/// @param[in] accumulator Pointer to the populated accumulator matrix. Must not
/// be NULL.
/// @param[in] threshold Minimum number of votes to consider a line. Must be
/// greater than 0.
/// @param[in] threshold Minimum number of votes to consider a line. Must be
/// greater than 0.
/// @param[in] theta_precision The angular precision in degrees. Must be
/// strictly positive.
/// @param[out] line_count Pointer to the variable that will hold the number of
/// lines in the array. Must not be NULL.
/// @return An array of pointers to `Line` structures representing detected
/// lines.
/// @throw Throws if `accumulator` or `line_count` is NULL or `threshold` is 0.
/// @note The returned array is dynamically allocated and should be freed by the
/// caller.
Line **extract_hough_lines(Matrix *accumulator, size_t threshold,
                           float theta_precision, size_t *line_count)
{
    if (accumulator == NULL)
        errx(EXIT_FAILURE, "The accumulator matrix is NULL");
    if (line_count == NULL)
        errx(EXIT_FAILURE, "The line_count output parameter is NULL");
    if (threshold == 0)
        errx(EXIT_FAILURE, "Threshold should not be 0 because it would "
                           "generate to much noise");

    size_t theta_max = mat_width(accumulator);
    size_t r_max = mat_height(accumulator);

    size_t max_lines = 20; // initial size of the lines alloc
    Line **lines = malloc(max_lines * sizeof(Line *));
    *line_count = 0;

    for (size_t r = 0; r < r_max; r++)
    {
        for (size_t theta_index = 0; theta_index < theta_max; theta_index++)
        {
            double count = mat_coef(accumulator, r, theta_index);
            // ignore values that are below the threshold
            if (count < threshold)
                continue;
            // increase the size of the array if the max number of lines is
            // reached
            if (*line_count == max_lines)
            {
                max_lines += 10;
                lines = realloc(lines, max_lines * sizeof(Line *));
                if (lines == NULL)
                    errx(EXIT_FAILURE, "Reallocation of lines array failed");
            }
            // add the line to the list of lines
            Line *line = malloc(sizeof(Line));
            lines[(*line_count)++] = line;
            line->r = r;
            line->theta = theta_index * theta_precision;
        }
    }
    return lines;
}

/// @brief Applies Non-Maximum Suppression (NMS) to a set of Hough lines.
///
/// For each line in the input array, all other lines that are "too close" in
/// both r and theta (within delta_r and delta_theta) are suppressed (removed).
/// The remaining lines are compacted in the input array. Lines that are
/// suppressed are freed.
///
/// @param[in,out] lines Array of pointers to Line structures. Must not be NULL.
///                     After the call, contains only the lines that survived
///                     NMS.
/// @param[in,out] line_count Pointer to the number of lines in the array. Must
/// be > 0 and not NULL.
///                           After the call, updated to reflect the number of
///                           remaining lines.
/// @param[in] delta_r Maximum allowed difference in r to consider two lines
/// similar. Must be >= 0.
/// @param[in] delta_theta Maximum allowed difference in theta to consider two
/// lines similar. Must be >= 0.
///
/// @return Pointer to the (same) input array `lines`, now containing only the
/// lines that survived NMS.
///
/// @throw Throws if `lines` is NULL.
/// @throw Throws if `line_count` is NULL or *line_count is 0.
/// @throw Throws if `delta_r` or `delta_theta` is negative.
///
/// @note The function frees memory for suppressed lines.
/// @note The function modifies the input array in-place.
Line **hough_lines_NMS(Line **lines, size_t *line_count, double delta_r,
                       double delta_theta)
{
    if (lines == NULL)
        errx(EXIT_FAILURE, "The Lines array is NULL");
    if (line_count == NULL || *line_count == 0)
        errx(EXIT_FAILURE, "The line count is NULL or 0");
    if (delta_r < 0 || delta_theta < 0)
        errx(EXIT_FAILURE, "delta_r and delta_theta must not be negative");

    int *suppressed = calloc(*line_count, sizeof(int));

    for (size_t i = 0; i < *line_count; i++)
    {
        // ignore if already suppressed line
        if (suppressed[i])
            continue;

        Line *li = lines[i];

        for (size_t j = i + 1; j < *line_count; j++)
        {
            // ignore if already suppressed line
            if (suppressed[j])
                continue;
            Line *lj = lines[j];

            // mark as suppressed the lines that are too close in r and theta
            // considering they are the same
            if (ABS(round(lj->r - li->r)) < delta_r &&
                ABS(round(lj->theta - li->theta)) < delta_theta)
            {
                suppressed[j] = TRUE;
            }
        }
    }

    // overwrite the lines with the filtered ones
    size_t rewrite_index = 0;
    for (size_t i = 0; i < *line_count; i++)
    {
        if (!suppressed[i])
            lines[rewrite_index++] = lines[i];
        else
        {
            // free the ones that are suppressed
            free(lines[i]);
        }
    }
    // the new number of lines is the number of non suppressed lines
    *line_count = rewrite_index;
    free(suppressed);
    return lines;
}

/// @brief Performs a Hough Transform on a source image to detect lines,
/// followed by Non-Maximum Suppression (NMS).
/// @param[in] src Pointer to the source binary image matrix. Must not be NULL.
/// @param[in] theta_precision Angular resolution in degrees for the Hough
/// accumulator. Must be strictly positive.
/// @param[in] delta_r Maximum allowed difference in r to consider two lines
/// similar for NMS. Must be >= 0.
/// @param[in] delta_theta Maximum allowed difference in theta to consider two
/// lines similar for NMS. Must be >= 0.
/// @param[out] size_out Pointer to the variable that will hold the number of
/// result lines. Must not be NULL.
/// @return An array of pointers to `Line` structures representing detected
/// lines.
/// @throw Throws if `src` or `size_out` is NULL or `threshold` is 0 or
/// `theta_precision <= 0`.
/// @note The returned array is dynamically allocated and should be freed by the
/// caller.
Line **hough_transform_lines(Matrix *src, float theta_precision, double delta_r,
                             double delta_theta, size_t *size_out)
{
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");
    if (size_out == NULL)
        errx(EXIT_FAILURE, "The size output parameter is NULL");

    Matrix *accumulator = create_hough_accumulator(
        mat_height(src), mat_width(src), theta_precision);

    size_t max_acc;
    populate_hough_lines(src, accumulator, theta_precision, &max_acc);

    float threshold = max_acc * 0.8;
    Line **lines =
        extract_hough_lines(accumulator, threshold, theta_precision, size_out);

    // filter lines that are too similar
    lines = hough_lines_NMS(lines, size_out, delta_r, delta_theta);

    mat_free(accumulator);

    return lines;
}

void print_lines(Line **lines, size_t size /*, size_t offset*/)
{
    for (size_t i = 0; i < size; i++)
    {
        double theta = lines[i]->theta;
        double r = lines[i]->r;

        printf("Line %zu : (%f, %f)\n", i, r, theta);

        /*
        size_t h_0 = r * cos(theta);
        size_t w_0 = r * sin(theta);

        size_t h_1 = h_0 + offset * (- sin(theta));
        size_t w_1 = w_0 + offset * cos(theta);

        size_t h_1 = h_0 - offset * (- sin(theta));
        size_t w_1 = w_0 - offset * cos(theta);
        */
        free(lines[i]);
    }
    free(lines);
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

    // size_t offset =
    //     (size_t)round(sqrt(mat_height(closing) * mat_height(closing) +
    //                        mat_width(closing) * mat_width(closing)));
    size_t nb_lines;
    Line **lines = hough_transform_lines(closing, 1, 3, 1, &nb_lines);

    print_lines(lines, nb_lines);
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