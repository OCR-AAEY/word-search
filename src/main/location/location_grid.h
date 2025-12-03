#ifndef LOCATION_GRID_H
#define LOCATION_GRID_H
#include "location/location.h"

/// @brief Extracts and saves individual grid cells from a source image matrix
/// based on intersection points.
/// @param[in] src Pointer to the source Matrix containing the image data.
/// @param[in] points 2D array of Point structures representing grid
/// intersections.
/// @param[in] height Number of rows in the grid points array.
/// @param[in] width Number of columns in the grid points array.
/// @throw Exits the program if any file operation or image region extraction
/// fails internally.
void extract_grid_cells(Matrix *src, Point **points, size_t height,
                        size_t width);

/// @brief Computes the bounding box that encloses a grid of intersection
/// points.
/// @param[in] points 2D array of Point structures representing grid
/// intersections.
/// @param[in] height Number of rows in the grid.
/// @param[in] width Number of columns in the grid.
/// @return Pointer to a newly allocated BoundingBox structure enclosing the
/// grid.
/// @throw Exits the program if both height and width are zero (no intersection
/// points provided).
BoundingBox *get_bounding_box_grid(Point **points, size_t height, size_t width);

#endif