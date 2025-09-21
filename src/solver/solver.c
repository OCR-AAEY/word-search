#include <err.h>
#include <stdlib.h>
#include <stdio.h>

#include "grid.h"
#include "solver.h"
#include "solvers/naive.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        errx(1, "Missing argument grid.");
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

    // Loading the grid.
    Grid grid = load_grid(file_name);

    int start_height = -1, start_width = -1, end_height = -1, end_width = -1;

    naive_solve(&grid, word, &start_height, &start_width, &end_height,
                &end_width);

    free_grid(&grid);

    print_result(start_height, start_width, end_height, end_width);

    return 0;
}

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
