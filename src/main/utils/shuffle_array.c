#include <stdlib.h>

#include "random.h"
#include "shuffle_array.h"

void shuffle_array(double *array, unsigned long length)
{
    for (unsigned long i = 0; i < length - 1; i++)
    {
        // A random index after i.
        unsigned long j = rand_ul_uniform_nm(i + 1, length - 1);
        // Swap the ith and jth elements.
        double tmp = array[j];
        array[j] = array[i];
        array[i] = tmp;
    }
}
