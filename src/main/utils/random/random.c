#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "random.h"

/// @brief Ensures that a seed for rand has been set at least once.
static void seed_once()
{
    static int has_been_seeded = 0;
    if (!has_been_seeded)
    {
        srand((unsigned int)time(NULL));
        has_been_seeded = 1;
    }
}

unsigned long rand_ul_uniform(unsigned long max)
{
    seed_once();

    if (max == 0)
        return 0;

    unsigned long r;
    unsigned long limit = ULONG_MAX - (ULONG_MAX % (max + 1));

    do
    {
        r = ((unsigned long)rand() << 0) | ((unsigned long)rand() << 15) |
            ((unsigned long)rand() << 30);
    } while (r >= limit);

    return r % (max + 1);
}

unsigned long rand_ul_uniform_nm(unsigned long min, unsigned long max)
{
    return min + rand_ul_uniform(max - min);
}

float rand_f_uniform()
{
    seed_once();

    unsigned long r = ((unsigned long)rand() << 0) |
                      ((unsigned long)rand() << 15) |
                      ((unsigned long)rand() << 30);
    return r / ((float)(ULONG_MAX));
}

float rand_f_uniform_m(float max) { return rand_f_uniform() * max; }

float rand_f_uniform_nm(float min, float max)
{
    return min + rand_f_uniform_m(max - min);
}

float rand_f_gaussian()
{
    float u = rand_f_uniform();
    float v = rand_f_uniform();
    // To avoid a log(0) call.
    if (u < 1e-10f)
        u = 1e-10f;

    return sqrt(-2 * log(u)) * cos(2 * M_PI * v);
}

float rand_d_normal(float mean, float stddev)
{
    return mean + stddev * rand_f_gaussian();
}
