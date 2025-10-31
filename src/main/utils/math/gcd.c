#include "gcd.h"

size_t gcd(size_t a, size_t b)
{
    while (b != 0)
    {
        size_t t = b;
        b = a % b;
        a = t;
    }

    return a;
}