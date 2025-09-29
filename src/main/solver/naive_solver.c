#include <stdio.h>
#include <stdlib.h>

#include "grid.h"
#include "naive_solver.h"

/// @brief Solve a word search problem using a naive algorithm (not optimized).
/// @param grid The grid to solve.
/// @param word The word to search in the grid.
/// @param start_height The height of the start point of the word if it is
/// found. Otherwise set to -1.
/// @param start_width The width of the start point of the word if it is found.
/// Otherwise set to -1.
/// @param end_height The height of the end point of the word if it is found.
/// Otherwise set to -1.
/// @param end_width The width of the end point of the word if it is found.
/// Otherwise set to -1.
/// @return Returns 1 if the sought word has been found, 0 otherwise.
int naive_solve(Grid *grid, char *word, int *start_height, int *start_width,
                int *end_height, int *end_width) {

    // Obtaining the sought word's length.
    size_t word_length = 0;
    while (*(word + word_length) != '\0')
        word_length++;

    // Obtaining the reversed version of the sought word.
    char *reversed_word = malloc(word_length * sizeof(char));
    for (size_t i = 0; i < word_length; i++) {
        *(reversed_word + i) = *(word + word_length - i - 1);
    }

    // Check horizontally.
    for (size_t h = 0; h < grid_height(grid); h++) {
        for (size_t w = 0; w < grid_width(grid) - word_length + 1; w++) {
            // Check forward.
            size_t i = 0;
            while (*(word + i) != '\0' &&
                   *(word + i) == get_char(grid, h, w + i)) {
                i++;
            }
            if (*(word + i) == '\0') {
                *start_height = h;
                *start_width = w;
                *end_height = h;
                *end_width = w + word_length - 1;
                free(reversed_word);
                return 1;
            }
            // Check backward.
            i = 0;
            while (*(reversed_word + i) != '\0' &&
                   *(reversed_word + i) == get_char(grid, h, w + i))
                i++;
            if (*(reversed_word + i) == '\0') {
                *start_height = h;
                *start_width = w + word_length - 1;
                *end_height = h;
                *end_width = w;
                free(reversed_word);
                return 1;
            }
        }
    }

    // Check vertically.
    for (size_t h = 0; h < grid_height(grid) - word_length + 1; h++) {
        for (size_t w = 0; w < grid_width(grid); w++) {
            // Check forward.
            size_t i = 0;
            while (*(word + i) != '\0' &&
                   *(word + i) == get_char(grid, h + i, w))
                i++;
            if (*(word + i) == '\0') {
                *start_height = h;
                *start_width = w;
                *end_height = h + word_length - 1;
                *end_width = w;
                free(reversed_word);
                return 1;
            }
            // Check backward.
            i = 0;
            while (*(reversed_word + i) != '\0' &&
                   *(reversed_word + i) == get_char(grid, h + i, w))
                i++;
            if (*(reversed_word + i) == '\0') {
                *start_height = h + word_length - 1;
                *start_width = w;
                *end_height = h;
                *end_width = w;
                free(reversed_word);
                return 1;
            }
        }
    }

    // Check diagonally.
    for (size_t h = 0; h < grid_height(grid) - word_length + 1; h++) {
        for (size_t w = 0; w < grid_width(grid) - word_length + 1; w++) {
            // Check forward.
            size_t i = 0;
            while (*(word + i) != '\0' &&
                   *(word + i) == get_char(grid, h + i, w))
                i++;
            if (*(word + i) == '\0') {
                *start_height = h;
                *start_width = w;
                *end_height = h + word_length - 1;
                *end_width = w + word_length - 1;
                free(reversed_word);
                return 1;
            }
            // Check backward.
            i = 0;
            while (*(reversed_word + i) != '\0' &&
                   *(reversed_word + i) == get_char(grid, h + i, w))
                i++;
            if (*(reversed_word + i) == '\0') {
                *start_height = h + word_length - 1;
                *start_width = w + word_length - 1;
                *end_height = h;
                *end_width = w;
                free(reversed_word);
                return 1;
            }
        }
    }

    // The word has not been found.
    *start_height = -1;
    *start_width = -1;
    *end_height = -1;
    *end_width = -1;
    free(reversed_word);
    return 0;
}
