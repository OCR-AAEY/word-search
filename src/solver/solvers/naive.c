#include <stdlib.h>

#include "../grid.h"
#include "naive.h"

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

    // Traversal of the grid.
    for (size_t h = 0; h < height(grid) - word_length; h++) {
        for (size_t w = 0; w < width(grid) - word_length; w++) {
            if (check_horizontal(grid, word, h, w)) {
                *start_height = h;
                *start_width = w;
                *end_height = h;
                *end_width = w + word_length;
                free(reversed_word);
                return 1;
            } else if (check_horizontal(grid, reversed_word, h, w)) {
                *start_height = h;
                *start_width = w + word_length;
                *end_height = h;
                *end_width = w;
                free(reversed_word);
                return 1;
            } else if (check_vertical(grid, word, h, w)) {
                *start_height = h;
                *start_width = w;
                *end_height = h + word_length;
                *end_width = w;
                free(reversed_word);
                return 1;
            } else if (check_vertical(grid, reversed_word, h, w)) {
                *start_height = h + word_length;
                *start_width = w;
                *end_height = h;
                *end_width = w;
                free(reversed_word);
                return 1;
            } else if (check_diagonal(grid, word, h, w)) {
                *start_height = h;
                *start_width = w;
                *end_height = h + word_length;
                *end_width = w + word_length;
                free(reversed_word);
                return 1;
            } else if (check_diagonal(grid, reversed_word, h, w)) {
                *start_height = h + word_length;
                *start_width = w + word_length;
                *end_height = h;
                *end_width = w;
                free(reversed_word);
                return 1;
            }
        }
    }

    *start_height = -1;
    *start_width = -1;
    *end_height = -1;
    *end_width = -1;

    free(reversed_word);
    return 0;
}

/// @brief Checks whether the given word is in the given grid at the given
/// position in the horizonal way.
/// @param grid The grid to search in.
/// @param word The word to search.
/// @param height The height position where to begin the search.
/// @param width The width position where to begin the search.
/// @return Returns 1 if the word has been matched, 0 otherwise.
int check_horizontal(Grid *grid, char *word, int height, int width) {
    size_t i = 0;
    while (*(word + i) != '\0' &&
           *(word + i) == get_char(grid, height, width + i))
        i++;
    return *(word + i) == '\0';
}

/// @brief Checks whether the given word is in the given grid at the given
/// position in the vertical way.
/// @param grid The grid to search in.
/// @param word The word to search.
/// @param height The height position where to begin the search.
/// @param width The width position where to begin the search.
/// @return Returns 1 if the word has been matched, 0 otherwise.
int check_vertical(Grid *grid, char *word, int height, int width) {
    size_t i = 0;
    while (*(word + i) != '\0' &&
           *(word + i) == get_char(grid, height + i, width))
        i++;
    return *(word + i) == '\0';
}

/// @brief Checks whether the given word is in the given grid at the given
/// position in the diagonal way.
/// @param grid The grid to search in.
/// @param word The word to search.
/// @param height The height position where to begin the search.
/// @param width The width position where to begin the search.
/// @return Returns 1 if the word has been matched, 0 otherwise.
int check_diagonal(Grid *grid, char *word, int height, int width) {
    size_t i = 0;
    while (*(word + i) != '\0' &&
           *(word + i) == get_char(grid, height + i, width + i))
        i++;
    return *(word + i) == '\0';
}
