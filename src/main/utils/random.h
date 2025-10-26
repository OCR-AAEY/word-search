#ifndef RANDOM_H
#define RANDOM_H

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* Pi */
#endif

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

/// @brief Generates a uniformly distributed double in the range [0, 1).
/// @return A random double greater than or equal to 0.0 and less than 1.0.
double rand_d_uniform();

/// @brief Generates a uniformly distributed double in the range [0, max).
/// @param max The maximum value (exclusive).
/// @return A random double greater than or equal to 0.0 and less than max.
double rand_d_uniform_m(double max);

/// @brief Generates a uniformly distributed double in the range [min, max).
/// @param min The minimum value (inclusive).
/// @param max The maximum value (exclusive).
/// @return A random double greater than or equal to min and less than max.
double rand_d_uniform_nm(double min, double max);

/// @brief Generates a standard Gaussian (normal) distributed random double. The
/// distribution has mean 0 and standard deviation 1.
/// @return A random double sampled from a standard normal distribution.
double rand_d_gaussian();

/// @brief Generates a Gaussian (normal) distributed random double with given
/// mean and standard deviation.
/// @param mean The mean of the normal distribution.
/// @param stddev The standard deviation of the normal distribution.
/// @return A random double sampled from a normal distribution with specified
/// mean and standard deviation.
double rand_d_normal(double mean, double stddev);

#endif
