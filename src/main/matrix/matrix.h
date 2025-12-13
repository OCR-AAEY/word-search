#ifndef MATIRX_H
#define MATIRX_H

#include <stdlib.h>

/// @brief A 2D matrix of single-precision floating point numbers.
typedef struct Matrix Matrix;

/// @brief Returns the height (number of rows) of the given matrix.
/// @param[in] m Pointer to the matrix.
/// @return The number of rows in the matrix.
size_t mat_height(const Matrix *m);

/// @brief Returns the width (number of columns) of the given matrix.
/// @param[in] m Pointer to the matrix.
/// @return The number of columns in the matrix.
size_t mat_width(const Matrix *m);

Matrix *mat_create(size_t height, size_t width);

/// @brief Creates an empty matrix (initialized with zeros) on the heap.
/// @param[in] height Number of rows in the new matrix (must be non-zero).
/// @param[in] width Number of columns in the new matrix (must be non-zero).
/// @return A pointer to a newly allocated zero-filled matrix.
/// @throw Terminates the program if height or width is zero, or if memory
/// allocation fails.
Matrix *mat_create_zero(size_t height, size_t width);

/// @brief Creates a  matrix initialized with teh given value on the heap.
/// @param[in] height Number of rows in the new matrix (must be non-zero).
/// @param[in] width Number of columns in the new matrix (must be non-zero).
/// @param[in] value The value to which the coefficients of the matrix will be
/// initialized. If value is 0, `mat_create_zero` should be used instead.
/// @return A pointer to a newly allocated matrix.
/// @throw Terminates the program if height or width is zero, or if memory
/// allocation fails.
Matrix *mat_create_filled(size_t height, size_t width, float value);

/// @brief Creates a new matrix using an existing array as its content.
/// @param[in] height Number of rows in the matrix (must be non-zero).
/// @param[in] width Number of columns in the matrix (must be non-zero).
/// @param[in] content Pointer to a row-major array of size height * width. It
/// is copied into the struct and not directly used.
/// @return A pointer to the new matrix structure.
/// @throw Terminates the program if memory allocation for the Matrix structure
/// fails.
Matrix *mat_create_from_arr(size_t height, size_t width, const float *content);

/// @brief Creates a new matrix filled with uniformly distributed random values.
/// Allocates a new matrix of size @p height × @p width, where each element is
/// independently sampled from a uniform distribution in the range [`min`,
/// `max`].
/// @param[in] height The number of rows in the matrix. Must be non-zero.
/// @param[in] width The number of columns in the matrix. Must be non-zero.
/// @param[in] min The lower bound (inclusive) of the uniform distribution.
/// @param[in] max The upper bound (inclusive) of the uniform distribution.
/// @return [out] A pointer to the newly allocated Matrix structure containing
/// the generated random values.
/// @throw Terminates the program with an error message if `height` or `width`
/// is zero or memory allocation fails for the matrix or its contents.
/// @note The caller is responsible for freeing the returned matrix using
/// `mat_free()` when it is no longer needed.
Matrix *mat_create_random_uniform(size_t height, size_t width, float min,
                                  float max);

/// @brief Creates a new matrix with random Gaussian-distributed elements.
/// @param[in] height Number of rows in the matrix (must be non-zero).
/// @param[in] width Number of columns in the matrix (must be non-zero).
/// @return A pointer to a new matrix initialized with Gaussian random values.
/// @throw Terminates the program if allocation fails or dimensions are invalid.
Matrix *mat_create_random_gaussian(size_t height, size_t width);

/// @brief Creates a new matrix filled with normally distributed random values.
/// This function allocates and initializes a new matrix of size `height ×
/// width`, where each element is drawn from a normal (Gaussian) distribution
/// with the specified mean and standard deviation.
/// @param[in] height The number of rows in the matrix. Must be non-zero.
/// @param[in] width The number of columns in the matrix. Must be non-zero.
/// @param[in] mean The mean (μ) of the normal distribution used for random
/// generation.
/// @param[in] stddev The standard deviation (σ) of the normal distribution.
/// @return [out] A pointer to a newly allocated `Matrix` structure filled with
/// random values following N(mean, stddev²).
/// @throw The program terminates with `errx(EXIT_FAILURE, ...)` if `height ==
/// 0` or `width
/// == 0` or memory allocation for the matrix or its content fails.
/// @note The caller is responsible for freeing the returned matrix using
/// `mat_free()`.
Matrix *mat_create_random_normal(size_t height, size_t width, float mean,
                                 float stddev);

/// @brief Frees a matrix and its associated memory.
/// @param[in] matrix Pointer to the matrix to be freed.
void mat_free(Matrix *matrix);

/// @brief Frees an array of dynamically allocated Matrix objects and the array
/// itself.
/// @param[in] array Pointer to an array of pointers to Matrix objects to be
/// freed.
/// @param[in] length Number of Matrix pointers in the array.
void mat_free_matrix_array(Matrix **array, size_t lentgh);

/// @brief Compares two matrices for equality.
/// @param[in] a Pointer to the first Matrix to compare.
/// @param[in] b Pointer to the second Matrix to compare.
/// @return Returns 1 if both matrices are equal, otherwise returns 0.
///
/// The function checks whether two matrices have the same dimensions
/// and identical element values. If both pointers refer to the same
/// Matrix instance, the function immediately returns 1.
int mat_eq(Matrix *a, Matrix *b, float epsilon);

/// @brief Creates a deep copy of a matrix (allocates a new matrix with the same
/// contents).
/// @param[in] src Pointer to the matrix to copy.
/// @return A pointer to a new matrix identical to the original.
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_deepcopy(const Matrix *src);

/// @brief For internal use. More efficient because it does not check for valid
/// parameters.
/// @param[in] m Pointer to the matrix.
/// @param[in] h Row index.
/// @param[in] w Column index.
/// @return Pointer to the coefficient at (h, w) without bounds checking.
float *mat_unsafe_coef_ptr(const Matrix *m, size_t h, size_t w);

/// @brief Returns the address of the coefficient at position (h, w).
/// @param[in] m Pointer to the matrix.
/// @param[in] h Row index (0 ≤ h < height).
/// @param[in] w Column index (0 ≤ w < width).
/// @return A pointer to the coefficient at position (h, w).
float *mat_coef_ptr(const Matrix *m, size_t h, size_t w);

/// @brief Returns the coefficient at position (h, w).
/// @param[in] m Pointer to the matrix.
/// @param[in] h Row index (0 ≤ h < height).
/// @param[in] w Column index (0 ≤ w < width).
/// @return The value of the coefficient at position (h, w).
float mat_coef(const Matrix *m, size_t h, size_t w);

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
void mat_inplace_addition(Matrix *a, const Matrix *b);

/// @brief Computes the element-wise subtraction of two matrices.
/// @param[in] a Pointer to the first Matrix (minuend).
/// @param[in] b Pointer to the second Matrix (subtrahend).
/// @return Pointer to a newly allocated Matrix containing the result of a - b.
Matrix *mat_subtraction(const Matrix *a, const Matrix *b);

/// @brief Performs element-wise subtraction of matrix b from matrix a in-place.
/// @param[in, out] a Pointer to the Matrix that will be modified (minuend).
/// @param[in] b Pointer to the Matrix to subtract (subtrahend).
void mat_inplace_subtraction(Matrix *a, const Matrix *b);

/// @brief Multiplies a matrix by a scalar value and returns the result.
/// @param[in] m Pointer to the input matrix.
/// @param[in] a Scalar value to multiply each element by.
/// @return A new matrix representing m * a.
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_scalar_multiplication(const Matrix *m, float a);

/// @brief Multiplies every coefficient of a matrix by a scalar value
/// (in-place).
/// @param[in,out] m Pointer to the matrix to be modified.
/// @param[in] a Scalar value to multiply each element by.
/// @return Pointer to the modified matrix (same as input).
void mat_inplace_scalar_multiplication(Matrix *m, float a);

/// @brief Computes the matrix product of two matrices.
/// @param[in] a Pointer to the left matrix (A).
/// @param[in] b Pointer to the right matrix (B).
/// @return A new matrix representing the result of A × B.
/// @throw Terminates the program if a->width != b->height or memory allocation
/// fails.
Matrix *mat_multiplication(const Matrix *a, const Matrix *b);

/// @brief Computes the Hadamard (element-wise) product of two matrices.
/// @param[in] a Pointer to the first Matrix.
/// @param[in] b Pointer to the second Matrix.
/// @return Pointer to a newly allocated Matrix containing the element-wise
/// product of a and b.
/// @throw Exits the program with an error if the matrices have mismatched
/// dimensions or if memory allocation for the result fails.
Matrix *mat_hadamard(const Matrix *a, const Matrix *b);

/// @brief Performs the Hadamard (element-wise) product of two matrices
/// in-place.
/// @param[in, out] a Pointer to the Matrix that will be modified.
/// @param[in] b Pointer to the Matrix to multiply with.
/// @throw Exits the program with an error if the matrices have mismatched
/// dimensions.
void mat_inplace_hadamard(Matrix *a, const Matrix *b);

Matrix *mat_relu(Matrix *m);

void mat_inplace_relu(Matrix *m);

Matrix *mat_relu_derivative(Matrix *m);

void mat_inplace_relu_derivative(Matrix *m);

// /// @brief Computes the element-wise sigmoid derivative of a matrix and
// returns
// /// a new matrix.
// /// @param[in] m Pointer to the input Matrix.
// /// @return Pointer to a newly allocated Matrix containing the sigmoid
// /// derivatives of each element.
// /// @throw Exits the program if memory allocation for the result fails.
// Matrix *mat_sigmoid_derivative(const Matrix *m);

// /// @brief Computes the element-wise sigmoid derivative of a matrix in-place.
// /// @param[in, out] m Pointer to the Matrix to be modified.
// void mat_inplace_sigmoid_derivative(Matrix *m);

// void mat_inplace_relu(Matrix *m);

Matrix *mat_relu_derivative(Matrix *m);

void mat_inplace_softmax(Matrix *m);

void mat_inplace_toggle(Matrix *m);

/// @brief Strips zeros around the matrix.
/// @param m The matrix to strip the zeros from.
/// @return A new heap allocated matrix.
Matrix *mat_strip_margins(const Matrix *m);

Matrix *mat_scale_to_28(const Matrix *m, float fill_value);

void mat_inplace_to_one_hot(Matrix *m);

// /// @brief Computes the mean squared error (MSE) between two matrices.
// /// @param[in] actual Pointer to the matrix containing actual values.
// /// @param[in] expected Pointer to the matrix containing expected values.
// /// @return The mean squared error between the two matrices as a float.
// /// @throw Exits the program with an error if the matrices have mismatched
// /// dimensions.
// float mat_mean_squared_error(Matrix *actual, Matrix *expected);

/// @brief Returns the transpose of a matrix as a new matrix.
/// @param[in] m Pointer to the input Matrix.
/// @return Pointer to a newly allocated Matrix containing the transpose of m.
/// @throw Exits the program if memory allocation for the result fails.
Matrix *mat_transpose(const Matrix *m);

// /// @brief Transposes a matrix in-place. Supports both square and non-square
// /// matrices.
// /// @param[in, out] m Pointer to the Matrix to be transposed in-place.
// /// @throw None. The function assumes the matrix pointer is valid and memory
// is
// /// properly allocated.
// void mat_inplace_transpose(Matrix *m);

// /// @brief Creates a flattened (1×N) copy of the given matrix (row vector).
// /// @param[in] m Pointer to the input matrix.
// /// @return A new 1-row matrix containing the flattened elements of the
// input. Matrix *mat_vertical_flatten(const Matrix *m);

/// @brief Flattens a matrix into a single column in-place (vertical flatten).
/// @param[in, out] m Pointer to the Matrix to be flattened.
void mat_inplace_vertical_flatten(Matrix *m);

// /// @brief Creates a flattened (N×1) copy of the given matrix (column
// vector).
// /// @param[in] m Pointer to the input matrix.
// /// @return A new 1-column matrix containing the flattened elements of the
// /// input.
// Matrix *mat_horizontal_flatten(const Matrix *m);

// /// @brief Flattens a matrix into a single row in-place (horizontal flatten).
// /// @param[in, out] m Pointer to the Matrix to be flattened.
// void mat_inplace_horizontal_flatten(Matrix *m);

/// @brief Normalizes a matrix so that the sum of all its elements equals 1.
/// @param[in] m Pointer to the input Matrix to be normalized.
/// @return Pointer to a newly allocated Matrix containing the normalized
/// values.
/// @throw Exits the program if memory allocation for the result fails or if the
/// sum of elements is zero.
Matrix *mat_normalize(const Matrix *m);

/// @brief Normalizes a matrix in-place so that the sum of all elements
/// equals 1.
/// @param[in,out] m Pointer to the matrix to normalize.
/// @note If the sum of elements is 0, this will cause division by zero.
void mat_inplace_normalize(Matrix *m);

/// @brief Applies a user-defined function element-wise to a matrix.
/// @param[in] m Pointer to the input matrix.
/// @param[in] f Function pointer taking a float and returning a float.
/// @return A new matrix where each element is f(original_element).
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_map(const Matrix *m, float (*f)(float));

/// @brief Applies a function to every element of a matrix (in place).
/// @param[in,out] m The matrix whose elements will be modified in place.
/// @param[in] f A pointer to a function that takes a float and returns a
/// float.
/// @note This function directly modifies the contents of m.
/// No new memory is allocated.
void mat_inplace_map(Matrix *m, float (*f)(float));

/// @brief Applies a user-defined function element-wise to a matrix, with access
/// to element indexes.
/// @param[in] m Pointer to the input matrix.
/// @param[in] f Function pointer taking (value, row_index, column_index) and
/// returning a float.
/// @return A new matrix where each element is f(original_value, row, col).
/// @throw Terminates the program if memory allocation fails.
Matrix *mat_map_with_indexes(const Matrix *m,
                             float (*f)(float, size_t, size_t));

/// @brief Applies a function to each element of a matrix (in place), passing
/// element coordinates. This function iterates over all elements of the matrix
/// @p m and applies the given function @p f to each element. The function @p f
/// receives the element's current value and its (row, column) coordinates, and
/// its return value replaces the element.
/// @param[in,out] m The matrix whose elements will be modified in place.
/// @param[in] f A pointer to a function taking the current element value, its
/// row index (h), and its column index (w), and returning the new value to
/// store at that position. Signature: `float f(float value, size_t h, size_t
/// w)`.
/// @note This function modifies @p m directly and does not allocate new memory.
/// Useful for applying coordinate-dependent transformations.
void mat_inplace_map_with_indexes(Matrix *m, float (*f)(float, size_t, size_t));

/// @brief Prints the contents of a matrix to stdout in a formatted 2D layout.
/// @param[in] m Pointer to the matrix to print.
/// @param[in] precision Number of decimal places to display for each element.
/// @throw Terminates the program if the matrix pointer is NULL.
void mat_print(const Matrix *m, unsigned int precision);

void mat_print_n_first(const Matrix *m, size_t n, unsigned int precision);

void mat_display(const Matrix *m);

Matrix *mat_load_from_fd(int fd);

Matrix *mat_load_from_file(const char *filename);

void mat_save_to_fd(const Matrix *m, int fd);

void mat_save_to_file(const Matrix *m, const char *filename);

size_t mat_max_h(const Matrix *m);

#endif
