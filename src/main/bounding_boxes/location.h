
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

/// @brief Extracts and saves individual grid cells from a source image matrix based on intersection points.
/// @param[in] src Pointer to the source Matrix containing the image data.
/// @param[in] points 2D array of Point structures representing grid intersections.
/// @param[in] height Number of rows in the grid points array.
/// @param[in] width Number of columns in the grid points array.
/// @throw Exits the program if any file operation or image region extraction fails internally.
void extract_grid_cells(Matrix *src, Point **points, size_t height,
                        size_t width);

/// @brief Computes the bounding box that encloses a grid of intersection points.
/// @param[in] points 2D array of Point structures representing grid intersections.
/// @param[in] height Number of rows in the grid.
/// @param[in] width Number of columns in the grid.
/// @return Pointer to a newly allocated BoundingBox structure enclosing the grid.
/// @throw Exits the program if both height and width are zero (no intersection points provided).
BoundingBox *get_bounding_box_grid(Point **points, size_t height, size_t width);

/// @brief Finds the largest remaining area outside a given bounding box within source image dimensions.
/// @param[in] grid_box Pointer to the BoundingBox structure representing the current area.
/// @param[in] src_height Height of the source image.
/// @param[in] src_width Width of the source image.
/// @return Pointer to a newly allocated BoundingBox representing the largest remaining area.
/// @throw Exits the program if the given bounding box exceeds source boundaries or no remaining space exists.
BoundingBox *find_biggest_remaining_area(BoundingBox *grid_box,
                                         size_t src_height, size_t src_width);

/// @brief Removes the extraction directory and all its contents.
/// @throw Exits the program if the folder removal command fails internally.
void cleanup_folders();

/// @brief Creates required working directories for grid extraction and processing.
/// @throw Exits the program if any directory creation command fails.
void setup_folders();

#endif