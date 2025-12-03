#ifndef LOCATION_H
#define LOCATION_H

#include "location/hough_lines_legacy.h"

/// @brief Represents a Bounding box by the top left and bottom right corner.
typedef struct BoundingBox
{
    /// The top-left point.
    Point tl;

    /// The bottom-right point.
    Point br;
} BoundingBox;

/// @brief Frees an array of BoundingBox previously allocated on the heap
/// @param boxes An array of pointers to BoundingBox to free
/// @param size The size of the array
void free_bboxes(BoundingBox **boxes, size_t size);

/// @brief Removes the extraction directory and all its contents.
/// @throw Exits the program if the folder removal command fails internally.
void cleanup_folders();

/// @brief Creates required working directories for grid extraction and
/// processing.
/// @throw Exits the program if any directory creation command fails.
void setup_folders();

/// @brief Create the folders dedicated to the export of words and letters
/// @param nb_words The number of folders to create corresponding to the number
/// of words to save
void setup_words_folders(size_t nb_words);

#endif