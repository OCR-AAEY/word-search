#include <criterion/criterion.h>

#include "utils/random/random.h"
#include "utils/random/shuffle_array.h"

Test(random, shuffle_array_1)
{
    for (size_t i = 0; i < 10000; i++)
    {
        int *array = calloc(50, sizeof(int));
        for (size_t j = 0; j < 50; j++)
            array[j] = (int)rand_ul_uniform_nm(3, 8);

        shuffle_array(array, sizeof(int), 50);
        for (size_t j = 0; j < 50; j++)
            cr_assert(3 <= array[j] && array[j] <= 8,
                      "Element at index %zu is not between 3 and 8 (got: %i).",
                      j, array[j]);

        free(array);
    }
}
