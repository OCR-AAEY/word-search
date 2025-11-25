#ifndef RANDOM_H
#define RANDOM_H

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* Pi */
#endif

void rand_seed();

/// @brief Generates a uniformly distributed unsigned long integer in the range
/// [0, max].
/// @param max The maximum value (inclusive).
/// @return A random unsigned long integer between 0 and max (inclusive).
unsigned long rand_ul_uniform(unsigned long max);

/// @brief Generates a uniformly distributed unsigned long integer in the range
/// [min, max].
/// @param min The minimum value (inclusive).
/// @param max The maximum value (inclusive).
/// @return A random unsigned long integer between min and max (inclusive).
unsigned long rand_ul_uniform_nm(unsigned long min, unsigned long max);

/// @brief Generates a uniformly distributed float in the range [0, 1).
/// @return A random float greater than or equal to 0.0 and less than 1.0.
float rand_f_uniform();

/// @brief Generates a uniformly distributed float in the range [0, max).
/// @param max The maximum value (exclusive).
/// @return A random float greater than or equal to 0.0 and less than max.
float rand_f_uniform_m(float max);

/// @brief Generates a uniformly distributed float in the range [min, max).
/// @param min The minimum value (inclusive).
/// @param max The maximum value (exclusive).
/// @return A random float greater than or equal to min and less than max.
float rand_f_uniform_nm(float min, float max);

/// @brief Generates a standard Gaussian (normal) distributed random float. The
/// distribution has mean 0 and standard deviation 1.
/// @return A random float sampled from a standard normal distribution.
float rand_f_gaussian();

/// @brief Generates a Gaussian (normal) distributed random float with given
/// mean and standard deviation.
/// @param mean The mean of the normal distribution.
/// @param stddev The standard deviation of the normal distribution.
/// @return A random float sampled from a normal distribution with specified
/// mean and standard deviation.
float rand_f_normal(float mean, float stddev);

#endif
