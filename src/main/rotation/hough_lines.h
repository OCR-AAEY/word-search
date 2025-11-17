#ifndef HOUGH_LINES_ROTATION_H
#define HOUGH_LINES_ROTATION_H

#include "matrix/matrix.h"

/// @brief Performs a Hough Transform on a source image to detect lines,
/// and returns the theta angle of the most probable line detected.
/// @param[in] src Pointer to the source binary image matrix. Must not be NULL.
/// @param[in] theta_precision Angular resolution in degrees for the Hough
/// accumulator. Must be strictly positive.
/// @return An array of pointers to `Line` structures representing detected
/// lines.
/// @throw Throws if `src` is NULL or `theta_precision <= 0`.
float hough_transform_lines_top_angle(Matrix *src, float theta_precision);

/// ============= internal functions ===============

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
Matrix *create_hough_accumulator_rotation(size_t height, size_t width,
                                 float theta_precision);
/// @brief Populates a Hough accumulator from a source image.
/// For each pixel that is black (`0`), increment the r values corresponding to
/// all theta angles possible.
/// @param[in] src Pointer to the source binary image matrix. Must not be NULL.
/// @param[in,out] accumulator Pointer to the accumulator matrix to populate.
/// Must not be NULL.
/// @param[in] theta_precision The angular precision in degrees. Must be
/// strictly positive.
/// @returns The theta angle of the line with the highest vote count in the
/// accumulator.
/// @throw Throws if `src` or `accumulator` are NULL.
float populate_hough_lines_top_angle(Matrix *src, Matrix *accumulator,
                                     float theta_precision);
#endif