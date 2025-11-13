#ifndef SHUFFLE_ARRAY_H
#define SHUFFLE_ARRAY_H

#include <stdlib.h>

/// @brief Shuffles the elements of a generic array in place using the
/// Fisher–Yates algorithm.
/// @param array Pointer to the array to shuffle.
/// @param elt_size Size (in bytes) of each element in the array.
/// @param length Number of elements in the array.
/// @throw Terminates the program if memory allocation for the temporary buffer
/// fails.
/// @note This function performs an in-place shuffle of the array by swapping
/// elements bytewise. It works with any data type (e.g., int, float, structs)
/// as long as elt_size is correctly specified.
void shuffle_array(void *array, size_t elt_size, size_t length);

#endif
