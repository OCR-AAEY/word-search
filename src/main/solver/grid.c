#include <err.h>
#include <stdio.h>

#include "grid.h"

/// @brief Represents a two-dimensional word search grid structure.
struct Grid
{
    /// @brief The height of the grid (represents the number of rows).
    size_t height;

    /// @brief The width of the grid (represents the number of columns).
    size_t width;

    /// @brief The one-dimensional character array representing the grid
    /// content. The grid is stored in row-major order.
    char *content;
};

/// @brief Compares two characters in a case-insensitive manner.
/// @param[in] a The first character to compare.
/// @param[in] b The second character to compare.
/// @return Returns a non-zero value (true) if both characters are equal when
/// compared case-insensitively; otherwise, returns 0 (false).
int ci_char_cmp(char a, char b)
{
    if ('A' <= a && a <= 'Z')
        a = a - 'A' + 'a';
    if ('A' <= b && b <= 'Z')
        b = b - 'A' + 'a';
    return a == b;
}

size_t grid_height(Grid *grid) { return grid->height; }

size_t grid_width(Grid *grid) { return grid->width; }

inline char *grid_unsafe_char_ptr(Grid *grid, size_t h, size_t w)
{
    size_t offset = h * grid->width + w;
    return grid->content + offset;
}

char *grid_char_ptr(Grid *grid, size_t h, size_t w)
{
    if (h >= grid->height)
        errx(EXIT_FAILURE,
             "Invalid given height: %zu whereas grid's height is %zu", h,
             grid->height);

    if (w >= grid->width)
        errx(EXIT_FAILURE,
             "Invalid given width: %zu whereas grid's width is %zu", w,
             grid->width);

    return grid_unsafe_char_ptr(grid, h, w);
}

char grid_char(Grid *grid, size_t h, size_t w)
{
    if (h >= grid->height)
        errx(EXIT_FAILURE,
             "Invalid given height: %zu whereas grid's height is %zu", h,
             grid->height);

    if (w >= grid->width)
        errx(EXIT_FAILURE,
             "Invalid given width: %zu whereas grid's width is %zu", w,
             grid->width);

    return *grid_unsafe_char_ptr(grid, h, w);
}

Grid *grid_load_from_file(char *file_name)
{
    FILE *file_stream = fopen(file_name, "r");

    if (file_stream == NULL)
    {
        errx(EXIT_FAILURE, "Failed to open file: %s", file_name);
    }

    // 25 is the minimum number of cells (5×5).
    char *array = malloc(25 * sizeof(char));
    if (array == NULL)
    {
        errx(EXIT_FAILURE, "Failed to allocate necessary memory.");
    }

    size_t array_length = 25;
    size_t offset = 0;

    int height = 0, width = 0;
    int row_length = 0;

    char c;

    while ((c = fgetc(file_stream)) != EOF)
    {
        if (c == '\n')
        {
            if (width == 0)
            {
                // First row has just finished being read.
                width = row_length;
            }
            else if (row_length != width)
            {
                free(array);
                errx(EXIT_FAILURE,
                     "Line %i is not of same length as the first line. "
                     "Expected %i and got %i",
                     height + 1, width, row_length);
            }

            height++;
            row_length = 0;
        }
        else if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
        {
            // Resize `array` if needed.
            if (offset >= array_length)
            {
                array_length += 10;
                char *tmp_ptr = realloc(array, array_length);
                if (tmp_ptr == NULL)
                {
                    errx(EXIT_FAILURE,
                         "Failed to reallocate memory for the grid array.");
                }
                array = tmp_ptr;
            }

            // Write to the grid array.
            *(array + offset) = c;
            offset++;
            row_length++;
        }
        else
        {
            free(array);
            errx(EXIT_FAILURE, "Invalid character found at line %i: '%c'.",
                 height + 1, c);
        }
    }

    if (width < 5)
    {
        free(array);
        errx(EXIT_FAILURE,
             "Given grid is too small: its width should be greater or equal to "
             "5 but got: %i.",
             width);
    }

    if (height < 5)
    {
        free(array);
        errx(
            EXIT_FAILURE,
            "Given grid is too small: its height should be greater or equal to "
            "5 but got: %i.",
            height);
    }

    fclose(file_stream);

    Grid *grid = malloc(sizeof(Grid));
    *grid = (Grid){.content = array, .height = height, .width = width};
    return grid;
}

void grid_free(Grid *grid)
{
    free(grid->content);
    free(grid);
}

void grid_print(Grid *grid)
{
    for (size_t h = 0; h < grid->height; h++)
    {
        for (size_t w = 0; w < grid->width; w++)
        {
            printf("%c%c", *grid_unsafe_char_ptr(grid, h, w),
                   w == grid->width - 1 ? '\n' : ' ');
        }
    }
}

int grid_solve_word(Grid *grid, char *word, int *start_height, int *start_width,
                    int *end_height, int *end_width)
{
    // Obtaining the sought word's length.
    size_t word_length = 0;
    while (*(word + word_length) != '\0')
        word_length++;

    // Obtaining the reversed version of the sought word.
    char *reversed_word = malloc(word_length * sizeof(char));
    for (size_t i = 0; i < word_length; i++)
    {
        *(reversed_word + i) = *(word + word_length - i - 1);
    }
    *(reversed_word + word_length - 1) = '\0';

    // Check horizontally.
    for (size_t h = 0; h < grid_height(grid); h++)
    {
        for (size_t w = 0; w < grid_width(grid) - word_length + 1; w++)
        {
            // Check forward.
            size_t i = 0;
            while (*(word + i) != '\0' &&
                   ci_char_cmp(*(word + i), grid_char(grid, h, w + i)))
            {
                i++;
            }
            if (*(word + i) == '\0')
            {
                // Success.
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
                   ci_char_cmp(*(reversed_word + i), grid_char(grid, h, w + i)))
            {
                i++;
            }
            if (*(reversed_word + i) == '\0')
            {
                // Success.
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
    for (size_t h = 0; h < grid_height(grid) - word_length + 1; h++)
    {
        for (size_t w = 0; w < grid_width(grid); w++)
        {
            // Check forward.
            size_t i = 0;
            while (*(word + i) != '\0' &&
                   ci_char_cmp(*(word + i), grid_char(grid, h + i, w)))
            {
                i++;
            }
            if (*(word + i) == '\0')
            {
                // Success.
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
                   ci_char_cmp(*(reversed_word + i), grid_char(grid, h + i, w)))
            {
                i++;
            }
            if (*(reversed_word + i) == '\0')
            {
                // Success.
                *start_height = h + word_length - 1;
                *start_width = w;
                *end_height = h;
                *end_width = w;
                free(reversed_word);
                return 1;
            }
        }
    }

    // Check diagonally (top-left to bottom-right).
    for (size_t h = 0; h < grid_height(grid) - word_length + 1; h++)
    {
        for (size_t w = 0; w < grid_width(grid) - word_length + 1; w++)
        {
            // Check forward.
            size_t i = 0;
            while (*(word + i) != '\0' &&
                   ci_char_cmp(*(word + i), grid_char(grid, h + i, w + i)))
            {
                i++;
            }
            if (*(word + i) == '\0')
            {
                // Success.
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
                   ci_char_cmp(*(reversed_word + i),
                               grid_char(grid, h + i, w + i)))
            {
                i++;
            }
            if (*(reversed_word + i) == '\0')
            {
                // Success.
                *start_height = h + word_length - 1;
                *start_width = w + word_length - 1;
                *end_height = h;
                *end_width = w;
                free(reversed_word);
                return 1;
            }
        }
    }

    // Check diagonally (top-right to bottom-left).
    for (size_t h = 0; h < grid_height(grid) - word_length + 1; h++)
    {
        for (size_t w = word_length - 1; w < grid_width(grid); w++)
        {
            // Check forward.
            size_t i = 0;
            while (*(word + i) != '\0' &&
                   ci_char_cmp(*(word + i), grid_char(grid, h + i, w - i)))
            {
                i++;
            }
            if (*(word + i) == '\0')
            {
                // Success.
                *start_height = h;
                *start_width = w;
                *end_height = h + word_length - 1;
                *end_width = w - word_length + 1;
                free(reversed_word);
                return 1;
            }
            // Check backward.
            i = 0;
            while (*(reversed_word + i) != '\0' &&
                   ci_char_cmp(*(reversed_word + i),
                               grid_char(grid, h + i, w - i)))
            {
                i++;
            }
            if (*(reversed_word + i) == '\0')
            {
                // Success.
                *start_height = h + word_length - 1;
                *start_width = w - word_length + 1;
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

int **grid_solve(Grid *grid, char **words, size_t word_len)
{
    int **res = malloc(word_len * sizeof(int *));
    if (res == NULL)
        errx(EXIT_FAILURE, "malloc failed");

    for (size_t i = 0; i < word_len; ++i)
    {
        res[i] = malloc(4 * sizeof(int));
        if (res[i] == NULL)
            errx(EXIT_FAILURE, "malloc failed");

        int e = grid_solve_word(grid, words[i], &res[i][0], &res[i][1],
                                &res[i][2], &res[i][3]);
        if (e == 0)
        {
            free(res[i]);
            res[i] = NULL;
        }
    }

    return res;
}
