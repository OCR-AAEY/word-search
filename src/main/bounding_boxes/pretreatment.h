#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#ifndef PRETREATMENT_H
#define PRETREATMENT_H

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"

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
Matrix *image_to_grayscale(ImageData *img);

/// @brief Converts a grayscale matrix to an RGB image.
/// @param[in] matrix Pointer to the input grayscale matrix (values 0.0–255.0).
/// @return Pointer to a newly allocated ImageData containing RGB pixels.
/// @throw Throws if allocations failed or the values in the matrix are not
/// valid
ImageData *pixel_matrix_to_image(Matrix *matrix);

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
Matrix *gaussian_blur(const Matrix *src, double sigma, size_t kernel_size);

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
                                         double c);

/// @brief Applies erosion to a matrix using a separable 2-pass approach
/// (horizontal then vertical).
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] kernel_size Size of the kernel. Even kernel sizes are supported.
/// @return Pointer to a newly allocated matrix containing the eroded image.
/// @throw Throws if src is NULL.
/// @note Caller is responsible for freeing the returned matrix using
/// mat_free().
Matrix *erosion(const Matrix *src, size_t kernel_size);

/// @brief Applies dilation to a matrix using a separable 2-pass approach
/// (horizontal then vertical).
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] kernel_size Size of the kernel. Even kernel sizes are supported.
/// @return Pointer to a newly allocated matrix containing the dilated image.
/// @throw Throws if src is NULL.
/// @note Caller is responsible for freeing the returned matrix using
/// mat_free().
Matrix *dilation(const Matrix *src, size_t kernel_size);

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
                        enum MorphTransform transform);

/// ============= internal functions ===============

/// @brief Converts a Pixel to grayscale using Rec.709 luminance weights.
///
/// This function applies the weighted sum formula:
/// Gray = 0.2126 * R + 0.7152 * G + 0.0722 * B
/// These coefficients correspond to the Rec.709 standard, which models
/// human perception by giving more weight to green and less to blue.
///
/// @param[in] pixel Pointer to the input RGB pixel.
/// @return Grayscale value in the range [0, 255].
uint8_t pixel_to_grayscale(Pixel *pixel);

/// @brief Computes the value of a 1D Gaussian function at (x).
///
/// @param[in] x The x-coordinate relative to the center of the kernel vector.
/// @param[in] sigma The standard deviation of the Gaussian.
/// @return The Gaussian value at the given coordinates.
double gaussian_function(int x, double sigma);

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
double *gaussian_kernel_1d(double sigma, size_t kernel_size);

/// @brief Clamp an integer value between a minimum and a maximum.
/// @param[in] value The integer value to clamp.
/// @param[in] min The minimum allowed value.
/// @param[in] max The maximum allowed value.
/// @return The clamped integer in the range [min, max].
/// @throw Throws if the minimum is strictly greater than the maximum
int clamp(int value, int min, int max);

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
                              size_t kernel_size);

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
                            size_t kernel_size);

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
                                enum Orientation orientation);

#endif