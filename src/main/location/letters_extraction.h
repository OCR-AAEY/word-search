#ifndef LETTERS_EXTRACTION_H
#define LETTERS_EXTRACTION_H

/// @brief Runs the full pipeline to locate, segment, and extract letters from
/// an input image, saving each processing step and each extracted letter as
/// PNG.
/// @param input_image Path to the input image file.
/// @return EXIT_SUCCESS (0) on success, or EXIT_FAILURE on any error occurring
///         during image loading, preprocessing, grid detection, word/letter
///         extraction, or file export.
/// @note This function performs cleanup/create of workspace folders, applies
///       preprocessing (grayscale, thresholding, deskewing, morphological ops),
///       detects grid lines, extracts words and letters, and exports all
///       intermediate results as PNGs.
int locate_and_extract_letters_png(const char *input_image);

#endif
