#ifndef GRID_H
#define GRID_H

/// @brief A struct representing a two-dimensional word search grid.
typedef struct {
    /// @brief The one-dimensional char array representing the two-dimensional
    /// word search grid.
    char *array;
    /// @brief The height of the grid. It has to be striclty positive.
    size_t height;
    /// @brief The width of the grid. It has to be striclty positive.
    size_t width;
} Grid;

Grid load_grid(char *file_name);

size_t height(Grid *grid);

size_t width(Grid *grid);

char get_char(Grid *grid, long height, long width);

void free_grid(Grid *grid);

#endif
