
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

void cleanup_folders();

void setup_folders();

#endif