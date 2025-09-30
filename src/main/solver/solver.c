#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "grid.h"
#include "naive_solver.h"
#include "solver.h"

/// @brief Edits the given string and makes it upper case.
/// @param[in,out] word The NULL-terminated array of character to make upper
/// case.
/// @throw Throws if the word contains any not letter character.
void to_upper_case(char *word) {
    size_t i = 0;
    while (*(word + i) != '\0') {
        if ('a' <= *(word + i) && *(word + i) <= 'z') {
            *(word + i) = *(word + i) - 'a' + 'A';
        } else if (*(word + i) < 'A' || 'Z' < *(word + i)) {
            errx(1, "Non-letter character found in the given word: '%c'.",
                 *(word + i));
        }
        i++;
    }
}

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
                  int end_width) {
    if (start_height < 0 || start_width < 0 || end_height < 0 ||
        end_width < 0) {
        printf("Not found\n");
    } else {
        printf("(%i,%i)(%i,%i)\n", start_width, start_height, end_width,
               end_height);
    }
}

#ifndef UNIT_TEST

int main(int argc, char **argv) {
    if (argc < 2) {
        errx(1, "Missing argument grid and word.");
    }
    if (argc < 3) {
        errx(1, "Missing argument word.");
    }
    // Not necessary. Can be removed.
    if (argc > 3) {
        errx(1, "Too many arguments.");
    }

    // Extract arguments.
    char *file_name = *(argv + 1);
    char *word = *(argv + 2);

    to_upper_case(word);

    Grid *grid = load_grid(file_name);

    int start_height, start_width, end_height, end_width;

    naive_solve(grid, word, &start_height, &start_width, &end_height,
                &end_width);

    free_grid(grid);

    print_result(start_height, start_width, end_height, end_width);

    return 0;
}

#endif
