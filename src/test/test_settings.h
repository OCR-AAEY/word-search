#ifndef TEST_SETTINGS_H
#define TEST_SETTINGS_H

/** The precesion with which double are compared using cr_*_float_eq. */
#define EPSILON 1E-6

/** The number of times random test will be repeated. */
#define REPEAT                                                                 \
    for (size_t repeat_counter = 0; repeat_counter < 100; repeat_counter++)

#endif