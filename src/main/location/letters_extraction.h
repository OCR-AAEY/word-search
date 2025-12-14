#ifndef LETTERS_EXTRACTION_H
#define LETTERS_EXTRACTION_H

#include "location/hough_lines_legacy.h"

/// @brief Runs the full pipeline to locate, segment, and extract letters from
/// an input image, saving each processing step and each extracted letter as
/// PNG.
/// @param[in] input_image Path to the input image file.
/// @param[out] out_intersection_points The output pointer that will get the 2d
/// array of intersection points. The caller is reponsible for freeing it.
/// @param[out] out_h_points The output pointer that will get the height of the
/// points 2d array.
/// @param[out] out_w_points The output pointer that will get the width of the
/// points 2d array.
/// @return EXIT_SUCCESS (0) on success, or EXIT_FAILURE on any error occurring
///         during image loading, preprocessing, grid detection, word/letter
///         extraction, or file export.
/// @note This function performs cleanup/create of workspace folders, applies
///       preprocessing (grayscale, thresholding, deskewing, morphological ops),
///       detects grid lines, extracts words and letters, and exports all
///       intermediate results as PNGs.
int locate_and_extract_letters_png(const char *input_image,
                                   Point ***out_intersection_points,
                                   size_t *out_h_points, size_t *out_w_points);

#endif
