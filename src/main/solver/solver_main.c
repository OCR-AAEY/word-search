#ifndef UNIT_TEST

#include <err.h>
#include <stdio.h>

#include "grid.h"

/// @brief Prints the result of the solver as required by the book of
/// specifications.
/// @param[in] start_height The height of the start position of the word. A
/// value of -1 indicatest that the word has not been found in the grid.
/// @param[in] start_width The width of the start position of the word. A value
/// of -1 indicatest that the word has not been found in the grid.
/// @param[in] end_height The height of the end position of the word. A value of
/// -1 indicatest that the word has not been found in the grid.
/// @param[in] end_width The width of the end position of the word. A value of
/// -1 indicatest that the word has not been found in the grid.
void print_result(int start_height, int start_width, int end_height,
                  int end_width)
{
    if (start_height < 0 || start_width < 0 || end_height < 0 || end_width < 0)
    {
        printf("Not found\n");
    }
    else
    {
        printf("(%i,%i)(%i,%i)\n", start_width, start_height, end_width,
               end_height);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        errx(EXIT_FAILURE, "Missing arguments grid and word.");
    }
    if (argc < 3)
    {
        errx(EXIT_FAILURE, "Missing argument word.");
    }
    if (argc > 3)
    {
        errx(EXIT_FAILURE, "Too many arguments.");
    }

    // Extract arguments.
    char *file_name = *(argv + 1);
    char *word = *(argv + 2);

    Grid *grid = grid_load_from_file(file_name);

    int start_h, start_w, end_h, end_w;

    grid_solve_word(grid, word, &start_h, &start_w, &end_h, &end_w);

    grid_free(grid);

    print_result(start_h, start_w, end_h, end_w);

    return 0;
}

#endif
