#include <criterion/criterion.h>

#include "utils/random/random.h"

Test(random, rand_ul_uniform_1)
{
    for (size_t i = 0; i < 10000; i++)
    {
        unsigned long r = rand_ul_uniform(500);
        cr_assert(r <= 500);
    }
}

Test(random, rand_ul_uniform_2)
{
    for (size_t i = 0; i < 10000; i++)
    {
        unsigned long r = rand_ul_uniform(10);
        cr_assert(r <= 10);
    }
}

Test(random, rand_ul_uniform_nm_1)
{
    for (size_t i = 0; i < 10000; i++)
    {
        unsigned long r = rand_ul_uniform_nm(4100, 5300);
        cr_assert(4100 <= r && r <= 5300, "Obtained %lu.", r);
    }
}
