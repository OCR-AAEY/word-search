#include <err.h>
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

void rand_seed() { srand((unsigned int)time(NULL)); }

unsigned long rand_ul_uniform(unsigned long max)
{
    if (max == 0)
        return 0;

    unsigned long limit = RAND_MAX - (RAND_MAX % (max + 1));
    unsigned long r;

    do
    {
        r = rand();
    } while (r >= limit);

    return r % (max + 1);
}

unsigned long rand_ul_uniform_nm(unsigned long min, unsigned long max)
{
    if (min > max)
        errx(EXIT_FAILURE, "rand_ul_uniform_nm: min is less than max.");

    return min + rand_ul_uniform(max - min);
}

float rand_f_uniform()
{
    // seed_once();

    return (float)rand() / ((float)RAND_MAX + 1.0f);
}

float rand_f_uniform_m(float max) { return rand_f_uniform() * max; }

float rand_f_uniform_nm(float min, float max)
{
    if (min > max)
        errx(EXIT_FAILURE, "rand_f_uniform_nm: min is less than max.");

    return min + rand_f_uniform_m(max - min);
}

float rand_f_gaussian()
{
    float u = rand_f_uniform();
    float v = rand_f_uniform();
    // To avoid a log(0) call.
    if (u < 1e-10f)
        u = 1e-10f;

    return sqrtf(-2.0f * logf(u)) * cosf(2.0f * M_PI * v);
}

float rand_f_normal(float mean, float stddev)
{
    return mean + stddev * rand_f_gaussian();
}
