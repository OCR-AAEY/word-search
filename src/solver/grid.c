#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "grid.h"

/// @brief Loads a grid from a given file and returns a pointer the the Grid
/// struct.
/// @param file_name The file to load as a word search grid.
/// @return Return the created grid.
Grid load_grid(char *file_name) {
    FILE *fp = fopen(file_name, "r");

    if (fp == NULL) {
        errx(1, "Failed to open file: %s", file_name);
    }

    char *array =
        malloc(25 * sizeof(char)); // 25 is the minimum number of cells (5×5)
    size_t array_length = 25;
    size_t offset = 0;

    int height = 0, width = 0;

    char c;
    while ((c = fgetc(fp)) != EOF) {
        if (offset >= array_length) {
            array_length = array_length + 25;
            char *ptr = realloc(array, array_length);
            if (ptr == NULL) {
                free(array);
                errx(1, "Failed to reallocate memory for the grid array while "
                        "building the first line.");
            }
            array = ptr;
        }

        if (c == '\n') {
            height++;
        } else {
            if (height == 0) {
                width++;
            }
            *(array + offset) = c;
        }
        offset++;
    }

    fclose(fp);

    *(array + offset) = '\0';

    Grid grid = {.array = array, .height = height, .width = width};
    return grid;
}

/// @brief Returns the height of the given Grid struct.
/// @param grid The struct of the sought height.
/// @return The height of the given Grid struct.
size_t height(Grid *grid) { return grid->height; }

/// @brief Returns the width of the given Grid struct.
/// @param grid The struct of the sought width.
/// @return The width of the given Grid struct.
size_t width(Grid *grid) { return grid->width; }

/// @brief Returns the char at the height height and the width width of the
/// given grid. If an error occurs (due to invalid height and width values for
/// instance), exits the program.
/// @param grid
/// @param height The height position (index of the row) from the top. Supports
/// negative indexes.
/// @param width The width position (index of the column) from the left.
/// Supports negative indexes.
/// @return A char located at the given position.
char get_char(Grid *grid, long height, long width) {
    size_t grid_height = grid->height;
    // Check if the given height is valid
    if (height < -(long)grid_height || height >= (long)grid_height) {
        errx(EXIT_FAILURE,
             "Invalid given height: %li whereas grid's height is %zu", height,
             grid_height);
    }

    size_t grid_width = grid->width;
    // Check if the given width is valid
    if (width < -(long)grid_width || width >= (long)grid_width) {
        errx(EXIT_FAILURE,
             "Invalid given width: %li whereas grid's width is %zu", width,
             grid_width);
    }

    // Convert negative indexes into positive ones
    if (height < 0) {
        height += (long)grid_height;
    }
    if (width < 0) {
        width += (long)grid_width;
    }

    size_t offset = (size_t)height * grid_height + (size_t)width;
    return *(grid->array + offset);
}

/// @brief Frees the data of the given grid. Note that this function does not
/// free the pointer to the grid itselft!
/// @param grid The grid to clear the data of.
void free_grid(Grid *grid) { free(grid->array); }