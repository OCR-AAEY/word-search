#ifndef HOUGH_LINES_ROTATION_H
#define HOUGH_LINES_ROTATION_H

#include "matrix/matrix.h"

/// @brief Performs a Hough Transform to detect the most probable line angle.
/// @param[in] src Source binary image matrix. Must not be NULL.
/// @param[in] theta_precision Angular resolution in degrees. Must be > 0.
/// @param[out] out_angle Output pointer for the detected angle (0–180°). Must
/// not be NULL.
/// @return 0 on success, non-zero error code on failure.
int hough_transform_find_peak_angle(Matrix *src, float theta_precision,
                                    float *out_angle);

/// ============= internal functions ===============

/// @brief Creates an empty Hough accumulator matrix.
///
/// The accumulator uses:
/// - columns for theta values (0–180° with a step of theta_precision)
/// - rows for r values (from -r_max to r_max), where r_max is the image
/// diagonal.
///
/// @param[in] height Height of the source image.
/// @param[in] width Width of the source image.
/// @param[in] theta_precision Angular resolution in degrees. Must be > 0.
/// @return A pointer to the allocated accumulator matrix, or NULL on error.
Matrix *create_hough_accumulator_rotation(size_t height, size_t width,
                                          float theta_precision);

/// @brief Populates the Hough accumulator and finds the theta angle with the
/// most votes.
/// @param[in] src Source binary image matrix. Must not be NULL.
/// @param[in] accumulator Preallocated Hough accumulator matrix. Must not be
/// NULL.
/// @param[in] theta_precision Angular resolution in degrees. Must be > 0.
/// @param[out] out_theta Output pointer for the most voted angle (0–180°). Must
/// not be NULL.
/// @return 0 on success, non-zero error code on failure.
int populate_acc_find_peak_theta(Matrix *src, Matrix *accumulator,
                                 float theta_precision, float *out_theta);
#endif