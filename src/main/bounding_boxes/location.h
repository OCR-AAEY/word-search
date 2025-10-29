
#ifndef LOCATION_H
#define LOCATION_H

#include "bounding_boxes/hough_lines.h"

/// @brief Represents a Bounding box by the top left and bottom right corner.
typedef struct BoundingBox
{
    /// The top-left point.
    Point tl;

    /// The bottom-right point.
    Point br;
} BoundingBox;

void extract_grid_cells(Matrix *src, Point **points, size_t height,
                        size_t width);

BoundingBox *get_bounding_box_grid(Point **points, size_t height, size_t width);

/// @brief Finds the largest remaining area outside a given bounding box within source image dimensions.
/// @param[in] grid_box Pointer to the BoundingBox structure representing the current area.
/// @param[in] src_height Height of the source image.
/// @param[in] src_width Width of the source image.
/// @return Pointer to a newly allocated BoundingBox representing the largest remaining area.
/// @throw Exits the program if the given bounding box exceeds source boundaries or no remaining space exists.
BoundingBox *find_biggest_remaining_area(BoundingBox *grid_box,
                                         size_t src_height, size_t src_width);

void cleanup_folders();

void setup_folders();

#endif