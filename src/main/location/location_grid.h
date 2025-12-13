#ifndef LOCATION_GRID_H
#define LOCATION_GRID_H
#include "location/location.h"

/// @brief Extracts and saves all grid cells defined by a 2D array of points.
/// For each adjacent pair of points, creates a cropped PNG image of the
/// corresponding grid cell and saves it in GRID_DIR.
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] points 2D array of grid intersection points. Must not be NULL.
/// @param[in] height Number of point rows.
/// @param[in] width Number of point columns.
/// @return 0 on success, or EXIT_FAILURE if saving any grid cell fails.
int extract_grid_cells(Matrix *src, Point **points, size_t height,
                       size_t width);

/// @brief Computes the bounding box that encloses a grid of intersection
/// points.
/// @param[in] points 2D array of Point structures representing grid
/// intersections. Must not be NULL.
/// @param[in] height Number of rows in the grid.
/// @param[in] width Number of columns in the grid.
/// @return Pointer to a newly allocated BoundingBox enclosing the grid, or NULL
/// if height or width is zero or on allocation failure.
BoundingBox *get_bounding_box_grid(Point **points, size_t height, size_t width);

#endif