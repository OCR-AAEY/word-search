#ifndef MATIRX_H
#define MATIRX_H

#include <stdlib.h>

/// @brief A 2D matrix of double-precision floating point numbers.
typedef struct Matrix Matrix;

/// @brief Returns the height (number of rows) of the given matrix.
/// @param[in] m Pointer to the matrix.
/// @return The number of rows in the matrix.
size_t mat_height(const Matrix *m);

/// @brief Returns the width (number of columns) of the given matrix.
/// @param[in] m Pointer to the matrix.
/// @return The number of columns in the matrix.
size_t mat_width(const Matrix *m);

/// @brief Creates an empty matrix (initialized with zeros) on the heap.
/// @param[in] height Number of rows in the new matrix (must be non-zero).
/// @param[in] width Number of columns in the new matrix (must be non-zero).
/// @return A pointer to a newly allocated zero-filled matrix.
/// @throw Terminates the program if height or width is zero, or if memory
/// allocation fails.
Matrix *mat_create_empty(size_t height, size_t width);

/// @brief Creates a new matrix using an existing array as its content.
/// @param[in] height Number of rows in the matrix (must be non-zero).
/// @param[in] width Number of columns in the matrix (must be non-zero).
/// @param[in,out] content Pointer to a heap-allocated, row-major array of size
/// height * width.
/// @return A pointer to the new matrix structure.
/// @throw Terminates the program if memory allocation for the Matrix structure
/// fails.
Matrix *mat_create_from_arr(size_t height, size_t width, double *content);

/// @brief Creates a new matrix with random Gaussian-distributed elements.
/// @param[in] height Number of rows in the matrix (must be non-zero).
/// @param[in] width Number of columns in the matrix (must be non-zero).
/// @return A pointer to a new matrix initialized with Gaussian random values.
/// @throw Terminates the program if allocation fails or dimensions are invalid.
Matrix *mat_create_gaussian_random(size_t height, size_t width);

/// @brief Creates a new matrix with random uniformly distributed elements.
/// @param[in] height Number of rows in the matrix (must be non-zero).
/// @param[in] width Number of columns in the matrix (must be non-zero).
/// @return A pointer to a new matrix initialized with uniform random values.
/// @throw Terminates the program if allocation fails or dimensions are invalid.
Matrix *mat_create_uniform_random(size_t height, size_t width);

/// @brief Frees a matrix and its associated memory.
/// @param[in] matrix Pointer to the matrix to be freed.
void mat_free(Matrix *matrix);

/// @brief Creates a deep copy of a matrix (allocates a new matrix with the same
/// contents).
/// @param[in] m Pointer to the matrix to copy.
/// @return A pointer to a new matrix identical to the original.
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_deepcopy(const Matrix *m);

/// @brief For internal use. More efficient because it does not check for valid
/// parameters.
/// @param[in] m Pointer to the matrix.
/// @param[in] h Row index.
/// @param[in] w Column index.
/// @return Pointer to the coefficient at (h, w) without bounds checking.
double *mat_unsafe_coef_ptr(const Matrix *m, size_t h, size_t w);

/// @brief Returns the address of the coefficient at position (h, w).
/// @param[in] m Pointer to the matrix.
/// @param[in] h Row index (0 ≤ h < height).
/// @param[in] w Column index (0 ≤ w < width).
/// @return A pointer to the coefficient at position (h, w).
double *mat_coef_ptr(const Matrix *m, size_t h, size_t w);

/// @brief Returns the coefficient at position (h, w).
/// @param[in] m Pointer to the matrix.
/// @param[in] h Row index (0 ≤ h < height).
/// @param[in] w Column index (0 ≤ w < width).
/// @return The value of the coefficient at position (h, w).
double mat_coef(const Matrix *m, size_t h, size_t w);

/// @brief Performs element-wise addition of two matrices.
/// @param[in] a Pointer to the first matrix.
/// @param[in] b Pointer to the second matrix (must have the same dimensions as
/// a).
/// @return A new matrix representing a + b.
/// @throw Terminates the program if the matrices have mismatched dimensions or
/// memory allocation fails.
Matrix *mat_addition(const Matrix *a, const Matrix *b);

/// @brief Performs in-place element-wise addition of two matrices.
/// @param[in,out] a Pointer to the matrix to modify (left-hand operand and
/// result).
/// @param[in] b Pointer to the matrix to add (must have the same dimensions as
/// a).
/// @throw Terminates the program if the matrices have mismatched dimensions.
void mat_inplace_addition(Matrix *a, Matrix *b);

/// @brief Multiplies a matrix by a scalar value and returns the result.
/// @param[in] m Pointer to the input matrix.
/// @param[in] a Scalar value to multiply each element by.
/// @return A new matrix representing m * a.
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_scalar_multiplication(const Matrix *m, double a);

/// @brief Multiplies every coefficient of a matrix by a scalar value
/// (in-place).
/// @param[in,out] m Pointer to the matrix to be modified.
/// @param[in] a Scalar value to multiply each element by.
/// @return Pointer to the modified matrix (same as input).
void mat_inplace_scalar_multiplication(Matrix *m, double a);

/// @brief Computes the matrix product of two matrices.
/// @param[in] a Pointer to the left matrix (A).
/// @param[in] b Pointer to the right matrix (B).
/// @return A new matrix representing the result of A × B.
/// @throw Terminates the program if a->width != b->height or memory allocation
/// fails.
Matrix *mat_multiplication(const Matrix *a, const Matrix *b);

/// @brief Applies the sigmoid function element-wise to the given matrix.
/// Sigmoid(x) = 1 / (1 + exp(-x))
/// @param[in] m Pointer to the input matrix.
/// @return A new matrix containing the sigmoid of each element of m.
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_sigmoid(const Matrix *m);

/// @brief Applies the sigmoid function element-wise to the given matrix
/// (in-place). Sigmoid(x) = 1 / (1 + exp(-x))
/// @param[in,out] m Pointer to the matrix to modify.
void mat_inplace_sigmoid(Matrix *m);

/// @brief Normalizes a matrix in-place so that the sum of all elements
/// equals 1.
/// @param[in,out] m Pointer to the matrix to normalize.
/// @note If the sum of elements is 0, this will cause division by zero.
void mat_inplace_normalize(Matrix *m);

/// @brief Creates a flattened (1×N) copy of the given matrix (row vector).
/// @param[in] m Pointer to the input matrix.
/// @return A new 1-row matrix containing the flattened elements of the input.
Matrix *mat_vertical_flatten(const Matrix *m);

/// @brief Creates a flattened (N×1) copy of the given matrix (column vector).
/// @param[in] m Pointer to the input matrix.
/// @return A new 1-column matrix containing the flattened elements of the
/// input.
Matrix *mat_horizontal_flatten(const Matrix *m);

/// @brief Applies a user-defined function element-wise to a matrix.
/// @param[in] m Pointer to the input matrix.
/// @param[in] f Function pointer taking a double and returning a double.
/// @return A new matrix where each element is f(original_element).
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_map(const Matrix *m, double (*f)(double));

/// @brief Applies a user-defined function element-wise to a matrix, with access
/// to element indexes.
/// @param[in] m Pointer to the input matrix.
/// @param[in] f Function pointer taking (value, row_index, column_index) and
/// returning a double.
/// @return A new matrix where each element is f(original_value, row, col).
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_map_with_indexes(const Matrix *m,
                             double (*f)(double, size_t, size_t));

/// @brief Prints the contents of a matrix to stdout in a formatted 2D layout.
/// @param[in] m Pointer to the matrix to print.
/// @param[in] precision Number of decimal places to display for each element.
/// @throw Terminates the program if the matrix pointer is NULL.
void mat_print(const Matrix *m, int precision);

#endif