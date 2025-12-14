#ifndef GRID_H
#define GRID_H

#include <stdlib.h>

typedef struct Grid Grid;

/// @brief Returns the height of the given Grid struct.
/// @param[in] grid The struct of the sought height.
/// @return The height of the given Grid struct.
size_t grid_height(Grid *grid);

/// @brief Returns the width of the given Grid struct.
/// @param[in] grid The struct of the sought width.
/// @return The width of the given Grid struct.
size_t grid_width(Grid *grid);

/// @brief Returns a pointer to the character at the specified position in the
/// grid. This function does not perform bounds checking on the provided
/// indices. It assumes that the caller ensures valid values for `h` and `w`.
/// @param[in] grid Pointer to the grid structure.
/// @param[in] h The row index (0-based).
/// @param[in] w The column index (0-based).
/// @return A pointer to the character located at position (`h`, `w`) in the
/// grid.
char *grid_unsafe_char_ptr(Grid *grid, size_t h, size_t w);

/// @brief Returns a pointer to the character at the specified position in the
/// grid, performing bounds checking to ensure safe access.
/// @param[in] grid Pointer to the grid structure to access.
/// @param[in] h The row index (0-based).
/// @param[in] w The column index (0-based).
/// @return A pointer to the character located at position (`h`, `w`) in the
/// grid.
/// @throw Exits the program with EXIT_FAILURE if `h` or `w` are out of bounds.
/// This function calls `errx()` internally to report invalid indices.
char *grid_char_ptr(Grid *grid, size_t h, size_t w);

/// @brief Returns the character at the specified position in the grid,
/// performing bounds checking to ensure safe access.
/// @param[in] grid Pointer to the grid structure to access.
/// @param[in] h The row index (0-based).
/// @param[in] w The column index (0-based).
/// @return The character located at position (`h`, `w`) in the grid.
/// @throw Exits the program with EXIT_FAILURE if `h` or `w` are out of bounds.
/// This function calls `errx()` internally to report invalid indices.
char grid_char(Grid *grid, size_t h, size_t w);

/// @brief Loads a grid from a given file and returns a pointer to the created
/// Grid struct. The returned Grid must be freed using `free_grid`.
/// @param[in] file_name The file to load as a word search grid.
/// @return Returns the created grid.
/// @throw Throws if any error occurs during the process including: file not
/// found, invalid char in file, grid too small, failed (re)allocation.
Grid *grid_load_from_file(char *file_name);

/// @brief Frees the data of the given grid as well as the given Grid pointer
/// itself.
/// @param[in] grid The grid to free.
void grid_free(Grid *grid);

/// @brief Prints the grid in the standard output.
/// @param[in] grid The pointer to the grid to print.
void grid_print(Grid *grid);

/// @brief Solve a word search problem using a naive algorithm (not optimized).
/// @param[in] grid The grid to solve.
/// @param[in] word The word to search in the grid. Note that it has to be upper
/// case.
/// @param[out] start_height The height of the start point of the word if it is
/// found. Otherwise set to -1.
/// @param[out] start_width The width of the start point of the word if it is
/// found. Otherwise set to -1.
/// @param[out] end_height The height of the end point of the word if it is
/// found. Otherwise set to -1.
/// @param[out] end_width The width of the end point of the word if it is found.
/// Otherwise set to -1.
/// @return Returns 1 if the sought word has been found, 0 otherwise.
int grid_solve_word(Grid *grid, char *word, int *start_height, int *start_width,
                    int *end_height, int *end_width);

/// @brief Solves a word grid for a list of words and returns their coordinates.
/// @param[in] grid Pointer to the Grid structure to search in (must not be
/// NULL).
/// @param[in] words Array of C strings representing the words to solve
/// (must not be NULL).
/// @param[in] word_len Number of words in the @p words array.
/// @return A dynamically allocated array of int pointers of length
/// @p word_len. Each entry is either:
///
/// - a pointer to an array of 4 integers describing the word position (start
/// height, start width, end height, end width), or
///
/// - NULL if the corresponding word was not found.
/// @throw Terminates the program if a memory allocation fails.
int **grid_solve(Grid *grid, char **words, size_t word_len);

#endif
