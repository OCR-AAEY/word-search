#include <limits.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "random.h"

/// @brief Ensures that a seed for rand has been set at least once.
static void seed_once() {
    static int has_been_seeded = 0;
    if (!has_been_seeded) {
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
    return min + rand_ul_uniform(max - min + 1);
}

double rand_d_uniform()
{
    seed_once();

    unsigned long r = ((unsigned long)rand() << 0) |
                      ((unsigned long)rand() << 15) |
                      ((unsigned long)rand() << 30);
    return r / ((double)(ULONG_MAX));
}

double rand_d_uniform_m(double max) { return rand_d_uniform() * max; }

double rand_d_uniform_nm(double min, double max)
{
    return min + rand_d_uniform_m(max - min);
}

double rand_d_gaussian()
{
    double u = rand_d_uniform();
    double v = rand_d_uniform();
    // To avoid a log(0) call.
    if (u < 1e-300)
        u = 1e-300;

    return sqrt(-2 * log(u)) * cos(2 * M_PI * v);
}

double rand_d_normal(double mean, double stddev)
{
    return mean + stddev * rand_d_gaussian();
}
