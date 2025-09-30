#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "grid.h"

/// @brief A struct representing a two-dimensional word search grid.
struct Grid {
    /// @brief The one-dimensional char array representing the two-dimensional
    /// word search grid.
    char *content;
    /// @brief The height of the grid. It has to be striclty positive.
    unsigned int height;
    /// @brief The width of the grid. It has to be striclty positive.
    unsigned int width;
};

/// @brief Indicates whether the given char is a lower case letter.
/// @param[in] c The character to test.
/// @return Returns 1 if the `c` is a lower case letter and 0 otherwise.
int is_alpha(char c) { return 'A' <= c && c <= 'Z'; }

/// @brief Loads a grid from a given file and returns a pointer to the created
/// Grid struct. The returned Grid must be freed using `free_grid`.
/// @param[in] file_name The file to load as a word search grid.
/// @return Returns the created grid.
/// @throw Throws if any error occurs during the process including: file not
/// found, invalid char in file, grid too small, failed (re)allocation.
Grid *load_grid(char *file_name) {
    FILE *file_stream = fopen(file_name, "r");

    if (file_stream == NULL) {
        errx(1, "Failed to open file: %s", file_name);
    }

    // 25 is the minimum number of cells (5×5).
    char *array = malloc(25 * sizeof(char));
    if (array == NULL) {
        errx(1, "Failed to allocate necessary memory.");
    }

    size_t array_length = 25;
    size_t offset = 0;

    int height = 0, width = 0;
    int row_length = 0;

    char c;

    // Reading the first line.
    while ((c = fgetc(file_stream)) != EOF) {
        if (c == '\n') {
            if (width == 0) {
                // First row has just finished being read.
                width = row_length;
            } else if (row_length != width) {
                errx(1,
                     "Line %i is not of same length as the first line. "
                     "Expected %i and got %i",
                     height + 1, width, row_length);
            }

            height++;
            row_length = 0;
        } else if (!is_alpha(c)) {
            errx(1, "Invalid character found at line %i: '%c'.", height + 1, c);
        } else {
            // Resize `array` if needed.
            if (offset >= array_length) {
                array_length += 10;
                char *tmp_ptr = realloc(array, array_length);
                if (tmp_ptr == NULL) {
                    errx(1, "Failed to reallocate memory for the grid array.");
                }
                array = tmp_ptr;
            }

            // Write to the grid array.
            *(array + offset) = c;
            offset++;
            row_length++;
        }
    }

    if (width < 5) {
        errx(1,
             "Given grid is too small: its width should be greater or equal to "
             "5 but got: %i.",
             width);
    }

    if (height < 5) {
        errx(1,
             "Given grid is too small: its height should be greater or equal to "
             "5 but got: %i.",
             height);
    }

    fclose(file_stream);

    Grid *grid = malloc(sizeof(Grid));
    *grid = (Grid){.content = array, .height = height, .width = width};
    return grid;
}

/// @brief Returns the content of the given Grid struct.
/// @param grid The struct to get the content of.
/// @return The content of the given Grid struct as an char array.
char* grid_content(Grid* grid) { return grid->content; }

/// @brief Returns the height of the given Grid struct.
/// @param grid The struct of the sought height.
/// @return The height of the given Grid struct.
size_t grid_height(Grid *grid) { return grid->height; }

/// @brief Returns the width of the given Grid struct.
/// @param grid The struct of the sought width.
/// @return The width of the given Grid struct.
size_t grid_width(Grid *grid) { return grid->width; }

/// @brief Returns the char at the height `height` and the width `width` of the
/// given grid. If an error occurs (due to invalid height and width values for
/// instance), exits the program.
/// @param grid
/// @param height The height position (index of the row) from the top. Supports
/// negative indexes.
/// @param width The width position (index of the column) from the left.
/// Supports negative indexes.
/// @return A char located at the given position.
/// @throw Throws if at least one of height and width are outside the bounds of the grid.
char get_char(Grid *grid, size_t height, size_t width) {
    size_t grid_height = grid->height;
    size_t grid_width = grid->width;
    
    // Check if the given height is valid
    if (height >= grid_height) {
        errx(EXIT_FAILURE,
             "Invalid given height: %zu whereas grid's height is %zu", height,
             grid_height);
    }

    // Check if the given width is valid
    if (width >= grid_width) {
        errx(EXIT_FAILURE,
             "Invalid given width: %zu whereas grid's width is %zu", width,
             grid_width);
    }

    size_t offset = (size_t)height * grid_width + (size_t)width;
    return *(grid->content + offset);
}

/// @brief Frees the data of the given grid as well as the given Grid pointer itselft.
/// @param grid The grid to free.
void free_grid(Grid *grid) { 
    free(grid->content);
    free(grid);
 }

/// @brief Prints the grid in the standard output.
/// @param grid The pointer to the grid to print.
void print_grid(Grid *grid) {
    for (size_t h = 0; h < grid->height; h++) {
        for (size_t w = 0; w < grid->width; w++) {
            printf("%c ", *(grid->content + h * grid->width + w));
        }
        printf("\n");
    }
}
