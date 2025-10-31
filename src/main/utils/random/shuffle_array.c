#include <err.h>
#include <string.h>

#include "random.h"
#include "shuffle_array.h"

void shuffle_array(void *array, size_t elt_size, size_t length)
{
    unsigned char *bytes = (unsigned char *)array;

    void *tmp = malloc(elt_size);
    if (tmp == NULL)
        errx(1, "Failed to allocate memory for internal temporary buffer.");

    for (size_t i = 0; i < length - 1; i++)
    {
        // A random index after i.
        size_t j = rand_ul_uniform_nm(i, length - 1);

        // Swap the ith and jth elements (copy bytewise)
        memcpy(tmp, bytes + j * elt_size, elt_size);
        memcpy(bytes + j * elt_size, bytes + i * elt_size, elt_size);
        memcpy(bytes + i * elt_size, tmp, elt_size);
    }

    free(tmp);
}
