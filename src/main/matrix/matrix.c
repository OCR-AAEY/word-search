#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

/// @brief A 2D matrix of double-precision floating point numbers.
struct Matrix
{
    /// @brief Number of rows (height) of the matrix.
    size_t height;
    /// @brief Number of columns (width) of the matrix.
    size_t width;
    /// @brief The matrix elements stored in a contiguous row-major array.
    double *content;
};

/// @brief Returns the height (number of rows) of the given matrix.
/// @param m Pointer to the matrix.
/// @return The number of rows in the matrix.
inline size_t mat_height(Matrix *m) { return m->height; }

/// @brief Returns the width (number of columns) of the given matrix.
/// @param m Pointer to the matrix.
/// @return The number of columns in the matrix.
inline size_t mat_width(Matrix *m) { return m->width; }

/// @brief Creates an empty matrix (initialized with zeros) on the heap.
/// @param height Number of rows in the new matrix (must be non-zero).
/// @param width Number of columns in the new matrix (must be non-zero).
/// @return A pointer to a newly allocated zero-filled matrix.
/// @throw Terminates the program if height or width is zero, or if memory
/// allocation fails.
Matrix *mat_create_empty(size_t height, size_t width)
{
    if (height == 0)
        errx(1,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(1,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    double *content = calloc(height * width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory for matrix content.");

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory for matrix struct.");

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

/// @brief Creates a new matrix using an existing array as its content.
/// @param height Number of rows in the matrix (must be non-zero).
/// @param width Number of columns in the matrix (must be non-zero).
/// @param content Pointer to a heap-allocated, row-major array of size height *
/// width.
/// @return A pointer to the new matrix structure.
/// @throw Terminates the program if memory allocation for the Matrix structure
/// fails.
Matrix *mat_create_from_arr(size_t height, size_t width, double *content)
{
    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory for matrix struct.");
    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

/// @brief Frees a matrix and its associated memory.
/// @param matrix Pointer to the matrix to be freed.
void mat_free(Matrix *matrix)
{
    free(matrix->content);
    free(matrix);
}

/// @brief Creates a deep copy of a matrix (allocates a new matrix with the same
/// contents).
/// @param m Pointer to the matrix to copy.
/// @return A pointer to a new matrix identical to the original.
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_deepcopy(Matrix *m)
{
    double *content = calloc(m->height * m->width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory for deepcopy content.");

    for (size_t i = 0; i < m->height * m->width; i++)
        *(content + i) = *(m->content + i);

    Matrix *new_m = malloc(sizeof(Matrix));
    if (new_m == NULL)
        errx(1, "Failed to allocate memory for matrix struct.");
    *new_m =
        (Matrix){.height = m->height, .width = m->width, .content = content};
    return new_m;
}

double *mat_internal_coef_addr(Matrix *m, size_t h, size_t w);

/// @brief For internal use. More efficient because it does not check for valid
/// parameters.
inline double *mat_internal_coef_addr(Matrix *m, size_t h, size_t w)
{
    return m->content + h * m->width + w;
}

/// @brief Returns the address of the coefficient at position (h, w).
/// @param m Pointer to the matrix.
/// @param h Row index (0 ≤ h < height).
/// @param w Column index (0 ≤ w < width).
/// @return A pointer to the coefficient at position (h, w).
inline double *mat_coef_addr(Matrix *m, size_t h, size_t w)
{
    if (h >= m->height)
        errx(1, "Invalid height given. Expected < %zu and got %zu.", m->height,
             h);
    if (w >= m->width)
        errx(1, "Invalid width given. Expected < %zu and got %zu.", m->width,
             w);
    return mat_internal_coef_addr(m, h, w);
}

/// @brief Returns the coefficient at position (h, w).
/// @param m Pointer to the matrix.
/// @param h Row index (0 ≤ h < height).
/// @param w Column index (0 ≤ w < width).
/// @return The value of the coefficient at position (h, w).
inline double mat_coef(Matrix *m, size_t h, size_t w)
{
    if (h >= m->height)
        errx(1, "Invalid height given. Expected < %zu and got %zu.", m->height,
             h);
    if (w >= m->width)
        errx(1, "Invalid width given. Expected < %zu and got %zu.", m->width,
             w);
    return *mat_internal_coef_addr(m, h, w);
}

/// @brief Performs element-wise addition of two matrices.
/// @param a Pointer to the first matrix.
/// @param b Pointer to the second matrix (must have the same dimensions as a).
/// @return A new matrix representing a + b.
/// @throw Terminates the program if the matrices have mismatched dimensions or
/// memory allocation fails.
Matrix *mat_addition(Matrix *a, Matrix *b)
{
    if (a->height != b->height)
        errx(1, "Matrix addition failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(1, "Matrix addition failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    double *content = calloc(a->height * a->width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory for matrix addition result.");

    for (size_t i = 0; i < a->height * a->width; i++)
    {
        *(content + i) = *(a->content + i) + *(b->content + i);
    }

    return mat_create_from_arr(a->height, a->width, content);
}

/// @brief Multiplies every coefficient of a matrix by a scalar value
/// (in-place).
/// @param[in,out] mat Pointer to the matrix to be modified.
/// @param[in] a Scalar value to multiply each element by.
/// @return Pointer to the modified matrix (same as input).
Matrix *mat_scalar_multiplication(Matrix *m, double a)
{
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(m->content + i) *= a;
    }

    return m;
}

/// @brief Computes the matrix product of two matrices.
/// @param a Pointer to the left matrix (A).
/// @param b Pointer to the right matrix (B).
/// @return A new matrix representing the result of A × B.
/// @throw Terminates the program if a->width != b->height or memory allocation
/// fails.
Matrix *mat_multiplication(Matrix *a, Matrix *b)
{
    if (a->width != b->height)
        errx(1, "Cannot multiply two matrices if the width of the first does "
                "not match the height of the second.");

    Matrix *m = mat_create_empty(a->height, b->width);

    for (size_t h = 0; h < m->height; h++)
    {
        for (size_t w = 0; w < m->width; w++)
        {
            for (size_t i = 0; i < a->width; i++)
            {
                *mat_internal_coef_addr(m, h, w) +=
                    *mat_internal_coef_addr(a, h, i) *
                    *mat_internal_coef_addr(b, i, w);
            }
        }
    }

    return m;
}

/// @brief Applies the sigmoid function element-wise to the given matrix.
/// Sigmoid(x) = 1 / (1 + exp(-x))
/// @param m Pointer to the input matrix.
/// @return A new matrix containing the sigmoid of each element of m.
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_sigmoid(Matrix *m)
{
    Matrix *res = mat_create_empty(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = 1.0 / (1.0 * exp(-*(m->content + i)));
    }

    return res;
}

/// @brief Applies a user-defined function element-wise to a matrix.
/// @param m Pointer to the input matrix.
/// @param f Function pointer taking a double and returning a double.
/// @return A new matrix where each element is f(original_element).
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_map(Matrix *m, double (*f)(double))
{
    Matrix *res = mat_create_empty(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = f(*(m->content + i));
    }

    return res;
}

/// @brief Prints the contents of a matrix to stdout in a formatted 2D layout.
/// @param m Pointer to the matrix to print.
/// @param precision Number of decimal places to display for each element.
void mat_print(Matrix *m, int precision)
{
    if (m == NULL)
    {
        errx(1, "Given matrix pointer is null.");
    }
    else
    {
        char fmt[16];
        snprintf(fmt, sizeof(fmt), "%%.%df", precision);

        for (size_t h = 0; h < m->height; h++)
        {
            for (size_t w = 0; w < m->width; w++)
            {
                printf(fmt, mat_coef(m, h, w));
                if (w < m->width - 1)
                    printf("  ");
            }
            printf("\n");
        }
    }
}
