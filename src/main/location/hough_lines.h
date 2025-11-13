#ifndef HOUGH_LINES_H
#define HOUGH_LINES_H

#include "matrix/matrix.h"

/// @brief Represents a line in polar coordinates.
typedef struct Line
{
    /// The distance from the origin to the line.
    float r;

    /// The angle of the line in degrees.
    float theta;
} Line;

/// @brief Represents a Point in cartesian coordinates.
typedef struct Point
{
    /// The width component, the x axis component.
    int x;

    /// The height component, the y axis component.
    int y;
} Point;

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
Line **hough_transform_lines(Matrix *src, float theta_precision, float delta_r,
                             float delta_theta, size_t *size_out);

/// @brief Computes intersection points of two groups of lines.
/// @param[in] lines Array of pointers to Line structures (must not be NULL).
/// @param[in] line_count Number of lines in the input array (must be > 0).
/// @param[out] height_out Stores the number of lines in the 2D array of
/// intersection points (must not be NULL).
/// @param[out] width_out Stores the number of columns in the 2D array of
/// intersection points (must not be NULL).
/// @return Dynamically allocated 2D array of Point.
/// @throw Exits if any pointer is NULL, lines have the same angle in both
/// groups, or memory allocation fails.
/// @note Caller must free the array using `free_points()` function.
Point **extract_intersection_points(Line **lines, size_t line_count,
                                    size_t *height_out, size_t *width_out);

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
Matrix *create_hough_accumulator(size_t height, size_t width,
                                 float theta_precision);

/// @brief Computes mean and standard deviation of a Hough accumulator.
/// @param[in] accumulator Pointer to the accumulator matrix (must not be NULL).
/// @param[out] stddev_out Pointer to store the standard deviation (must not be
/// NULL).
/// @param[out] mean_out Pointer to store the mean (must not be NULL).
/// @return void
/// @throw Throws if any pointer is NULL.
void statistics_on_accumulator(Matrix *accumulator, float *stddev_out,
                               float *mean_out);

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
                          float theta_precision, size_t *max_count);

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
                           float theta_precision, size_t *line_count);

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
Line **hough_lines_NMS(Line **lines, size_t *line_count, float delta_r,
                       float delta_theta);

/// @brief Prints an array of lines with their r and theta values.
/// @param[in] lines Array of pointers to Line structures.
/// @param[in] size Number of lines in the array.
void print_lines(Line **lines, size_t size /*, size_t offset*/);

/// @brief Frees memory allocated for an array of lines.
/// @param[in] lines Array of pointers to Line structures.
/// @param[in] size Number of lines in the array.
void free_lines(Line **lines, size_t size);

/// @brief Inserts a line into a dynamically growing group of lines.
/// @param[in] line Pointer to the Line to insert (must not be NULL).
/// @param[out] lines_group Pointer to the array of Line pointers (may be
/// reallocated).
/// @param[out] lines_count Pointer to the current number of lines in the group.
/// @param[out] max_group Pointer to the current allocated capacity of the
/// group.
/// @return void
/// @throw Exits if memory allocation fails.
void insert_line_in_group(Line *line, Line ***lines_group, size_t *lines_count,
                          size_t *max_group);

/// @brief Splits an array of lines into two groups based on their theta values.
///
/// The function groups lines with the same theta into one of the two output
/// arrays. It assumes that all lines have at most two distinct theta values.
/// Exits with error if a third distinct theta is encountered.
///
/// @param[in,out] lines Array of pointers to Line structures (must not be
/// NULL).
/// @param[in] line_count Number of lines in the input array (must be > 0).
/// @param[out] lines_1 Output pointer to the first group of lines (will be
/// allocated).
/// @param[out] lines_1_count Pointer to store the number of lines in the first
/// group.
/// @param[out] lines_2 Output pointer to the second group of lines (will be
/// allocated).
/// @param[out] lines_2_count Pointer to store the number of lines in the second
/// group.
/// @return void
/// @throw Exits if any pointer is NULL, there is not lines, or more than two
/// theta values exist.
void split_lines(Line **lines, size_t line_count, Line ***lines_1,
                 size_t *lines_1_count, Line ***lines_2, size_t *lines_2_count);

/// @brief Prints a 2D array of points.
/// @param[in] points 2D array of Point structures.
/// @param[in] height Number of lines in the 2D array.
/// @param[in] width Number of columns in the 2D array.
void print_points(Point **points, size_t height, size_t width);

/// @brief Frees a 2D array of points.
/// @param[in,out] points A 2D array of Point structures.
/// Each Point array is freed.
/// @param[in] height Number of lines in the 2D array.
/// @note The function frees both the row Point arrays and
/// the array itself.
void free_points(Point **points, size_t height);

#endif