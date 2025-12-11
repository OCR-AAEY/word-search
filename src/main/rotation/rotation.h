#ifndef ROTATION_H
#define ROTATION_H

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"

/// @brief Rotates a grayscale matrix by a given angle.
///
/// Rotates the source matrix by the specified angle in degrees. Positive
/// values rotate clockwise. The result is a new matrix containing the rotated
/// grayscale image.
///
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] angle Rotation angle in degrees.
/// @return Pointer to a new matrix with the rotated data, or NULL on failure.
Matrix *rotate_matrix(const Matrix *src, float angle);

/**
 * @brief Rotates a full color image by a specified angle.
 *
 * This function rotates the given ImageData structure by the specified angle in
 * degrees. The resulting ImageData is a newly allocated image containing the
 * rotated pixels.
 *
 * @param img Pointer to the source ImageData to rotate.
 * @param angle Rotation angle in degrees. Positive values rotate clockwise.
 * @return Pointer to a new ImageData containing the rotated image.
 *         Returns NULL on failure (e.g., memory allocation error).
 */
ImageData *rotate_image(ImageData *img, float angle);

/// @brief Deskews automatically the image, using hough lines to detect the
/// rotated angle of the grid.
/// @param img Pointer to the matrix of the image to rotate automatically
/// @return Pointer to a newly allocated matrix with the deskewed image,
/// or NULL on failure.
Matrix *auto_deskew_matrix(Matrix *img);

#endif
