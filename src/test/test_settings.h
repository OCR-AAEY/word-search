#ifndef TEST_SETTINGS_H
#define TEST_SETTINGS_H

/** The precesion with which float are compared using cr_*_float_eq. */
#define EPSILON 1E-6f

#ifndef TEST_ITERATION
#define TEST_ITERATION 100
#endif

/** The number of times random test will be repeated. */
#define REPEAT                                                                 \
    for (size_t repeat_counter = 0; repeat_counter < TEST_ITERATION; repeat_counter++)

#endif