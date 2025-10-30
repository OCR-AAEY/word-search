#ifndef ROTATION_H
#define ROTATION_H

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "bounding_boxes/pretreatment.h"

/**
 * @brief Rotates a grayscale matrix by a specified angle.
 *
 * This function rotates the given source matrix by the specified angle in degrees.
 * The resulting matrix represents the rotated image in grayscale.
 *
 * @param src Pointer to the source Matrix to rotate.
 * @param angle Rotation angle in degrees. Positive values rotate clockwise.
 * @return Pointer to a new Matrix containing the rotated data.
 *         Returns NULL on failure (e.g., memory allocation error).
 */
Matrix *rotate_matrix(const Matrix *src, double angle);

/**
 * @brief Rotates a full color image by a specified angle.
 *
 * This function rotates the given ImageData structure by the specified angle in degrees.
 * The resulting ImageData is a newly allocated image containing the rotated pixels.
 *
 * @param img Pointer to the source ImageData to rotate.
 * @param angle Rotation angle in degrees. Positive values rotate clockwise.
 * @return Pointer to a new ImageData containing the rotated image.
 *         Returns NULL on failure (e.g., memory allocation error).
 */
ImageData *rotate_image(ImageData *img, double angle);

#endif
