#include "pretreatment.h"
#include <math.h>
#include <stdlib.h>
#include <err.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

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
    return 0.2126 * pixel->r + 0.7152 * pixel->g + 0.0722 * pixel->b;
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
    Matrix *grayscaled_pixels = create_empty_matrix(img->height, img->width);
    for (size_t h = 0; h < img->height; h++)
    {
        for (size_t w = 0; w < img->width; w++)
        {
            double *gray_scaled_pixel = get_coef_addr(grayscaled_pixels, h, w);
            *gray_scaled_pixel = pixel_to_grayscale(get_pixel(img, h, w));
        }
    }
    return grayscaled_pixels;
}

/// @brief Converts a grayscale matrix to an RGB image.
/// @param[in] matrix Pointer to the input grayscale matrix (values 0.0–255.0).
/// @return Pointer to a newly allocated ImageData containing RGB pixels.
/// @throw Throws if allocations failed or the values in the matrix are not valid
ImageData *pixel_matrix_to_image(Matrix *matrix)
{
    ImageData *img = malloc(sizeof(ImageData));
    if (img == NULL)
        errx(EXIT_FAILURE, "Failed to allocate the ImageData struct");

    size_t height = matrix_height(matrix);
    size_t width = matrix_width(matrix);
    Pixel *pixels = malloc(height * width * sizeof(Pixel));

    if (pixels == NULL)
    {
        free(img);
        errx(EXIT_FAILURE, "Failed to allocate the pixels array");
    }

    for (size_t h = 0; h < img->height; h++)
    {
        for (size_t w = 0; w < img->width; w++)
        {
            double gray_scaled_pixel = get_coef(matrix, h, w);

            if (gray_scaled_pixel < 0 || gray_scaled_pixel > 255)
            {
                free(img);
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

/// @brief Computes the value of a 2D Gaussian function at (x, y).
///
/// @param[in] x The x-coordinate relative to the center of the kernel.
/// @param[in] y The y-coordinate relative to the center of the kernel.
/// @param[in] sigma The standard deviation of the Gaussian.
/// @return The Gaussian value at the given coordinates.
double gaussian_function(int x, int y, double sigma)
{
    double res = 1 / (2 * M_PI * sigma * sigma);
    res *= exp(-(x * x + y * y) / (2 * sigma * sigma));
    return res;
}

/// @brief Fills a square matrix with values of a 2D Gaussian kernel.
///
/// The kernel is centered, and its values are computed using the standard
/// 2D Gaussian function. The matrix is not normalized; normalization should
/// be done separately if needed.
///
/// @param[in,out] kernel Pointer to a pre-allocated square matrix of size
/// kernel_size x kernel_size.
///                     The function fills this matrix with Gaussian values.
/// @param[in] sigma The standard deviation of the Gaussian.
/// @param[in] kernel_size The size of the kernel. Must be odd; even sizes
/// return NULL.
/// @return The same pointer to the filled matrix, or NULL if kernel_size is
/// incorrect (even).
Matrix *gaussian_kernel(Matrix *kernel, double sigma, size_t kernel_size)
{
    if (kernel_size % 2 == 0)
    {
        return NULL;
    }
    int m = (kernel_size - 1) / 2;
    for (int i = -m; i <= m; i++)
    {
        for (int j = -m; j <= m; j++)
        {
            double *cell = get_coef_addr(kernel, i, j);
            *cell = gaussian_function(i, j, sigma);
        }
    }
    return kernel;
}

/// @brief Computes the sum of all coefficients in a matrix.
///
/// @param[in] mat Pointer to the input matrix whose elements are to be summed.
/// @return The sum of all coefficients in the matrix.
double sum_matrix_coefs(Matrix *mat)
{
    size_t height = matrix_height(mat);
    size_t width = matrix_width(mat);
    double res = 0;
    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            res += get_coef(mat, h, w);
        }
    }
    return res;
}

/// @brief Multiplies every coefficient of a matrix by a scalar value.
///
/// @param[in,out] mat Pointer to the matrix to be modified.
/// @param[in] a The scalar value to multiply with each matrix element.
/// @return Pointer to the modified matrix (same as input).
Matrix *scalar_multiplication(Matrix *mat, double a)
{
    size_t height = matrix_height(mat);
    size_t width = matrix_width(mat);
    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double *cell = get_coef_addr(mat, h, w);
            *cell = a * *cell;
        }
    }
    return mat;
}

/// @brief Normalizes inplace a Gaussian kernel so that the sum of all
/// coefficients equals 1.
///
/// This function modifies the input matrix by dividing each element by the
/// total sum of all coefficients. Useful to preserve image brightness after
/// convolution.
///
/// @param[in,out] g Pointer to the Gaussian kernel matrix to be normalized.
/// @return Pointer to the normalized matrix (same as input).
Matrix *gaussian_normalize(Matrix *g)
{
    return scalar_multiplication(g, 1 / sum_matrix_coefs(g));
}

/// @brief Clamp an integer value between a minimum and a maximum.
/// 
/// 
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
    return max;
}

/// @brief Convolves an image with a Gaussian kernel to produce a blurred image.
///
/// Each pixel in the output image is computed as the weighted sum of
/// neighboring pixels in the input image, using the provided Gaussian kernel.
/// Out-of-bounds pixels are handled by clamping to the nearest valid pixel
/// (edge repetition).
///
/// @param[in] g Pointer to the Gaussian kernel matrix (kernel_size x
/// kernel_size).
/// @param[in] image Pointer to the input image matrix to be blurred.
/// @param[in] kernel_size Size of the square Gaussian kernel (must be odd).
/// @return Pointer to a newly allocated matrix containing the blurred image, or
/// NULL if kernel_size is even.
Matrix *gaussian_convolute(Matrix *g, Matrix *image, size_t kernel_size)
{
    if (kernel_size % 2 == 0)
        return NULL;

    int m = (kernel_size - 1) / 2;
    size_t height = matrix_height(image);
    size_t width = matrix_width(image);
    Matrix *blured = create_empty_matrix(height, width);

    for (size_t x = 0; x < height; x++)
        for (size_t y = 0; y < width; y++)
        {
            double *blured_pixel = get_coef_addr(blured, x, y);
            for (int i = -m; i <= m; i++)
            {
                for (int j = -m; j <= m; j++)
                {
                    // clamp will treat indexes out of bound to stay at the last
                    // acceptable value. This will repeat the pixel inside the
                    // image to fill the missing pixels.
                    double image_pixel =
                        get_coef(image, clamp(x + i, 0, height - 1),
                                 clamp(y + j, 0, width - 1));

                    // i and j are indexes from the center of the kernel
                    // by adding m we make sure that we use the indexing method
                    // of matrices to get the correct coef.
                    double g_i_j = get_coef(g, i + m, j + m);
                    *blured_pixel += g_i_j * image_pixel;
                }
            }
        }
    return blured;
}

/// @brief Applies a Gaussian blur to an input image.
///
/// This function creates a Gaussian kernel of the specified size and standard
/// deviation, normalizes it, and then convolves it with the input image to
/// produce a blurred output. The input image is not modified. The kernel matrix
/// is automatically freed after convolution.
///
/// @param[in] pixels Pointer to the input image matrix to be blurred.
/// @param[in] sigma The standard deviation of the Gaussian blur.
/// @param[in] kernel_size Size of the square Gaussian kernel (must be odd).
/// @return Pointer to a newly allocated matrix containing the blurred image,
///         or NULL if kernel_size is even.
Matrix *gaussian_blur(Matrix *pixels, double sigma, size_t kernel_size)
{
    if (kernel_size % 2 == 0)
    {
        return NULL;
    }

    // this is our kernel
    Matrix *g = create_empty_matrix(kernel_size, kernel_size);

    // we transform it into a gaussian kernel matrix
    g = gaussian_kernel(g, sigma, kernel_size);
    // we normalize it to have the sum of the weights = 1
    g = gaussian_normalize(g);
    Matrix *blured = gaussian_convolute(g, pixels, kernel_size);
    free_matrix(g);
    return blured;
}


#ifndef UNIT_TEST

int main() {
    ImageData *img = load_image("assets/sample_images/level_1_image_1.png");
    Matrix *gray_img = image_to_grayscale(img);
    Matrix *blured = gaussian_blur(gray_img, 2, 7);
    ImageData *final_img = pixel_matrix_to_image(blured);
    GdkPixbuf *pixbuf = create_pixbuf_from_image_data(final_img);
    save_pixbuf_to_png(pixbuf, "pretreatement.png", NULL);
    g_object_unref(pixbuf);
    free_image(img);
    free_matrix(gray_img);
    free_matrix(blured);
    free_image(final_img);

    return EXIT_SUCCESS;
}

#endif