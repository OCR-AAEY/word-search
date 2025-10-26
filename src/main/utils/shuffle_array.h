#ifndef SHUFFLE_ARRAY_H
#define SHUFFLE_ARRAY_H

/// @brief Shuffles the elements of a double array in place using the
/// Fisher-Yates algorithm.
/// @param array Pointer to the array of doubles to be shuffled.
/// @param length The number of elements in the array.
/// @note This function modifies the original array.
/// @throw Assumes that the array pointer is valid and length > 0.
void shuffle_array(double *array, unsigned long length);

#endif
