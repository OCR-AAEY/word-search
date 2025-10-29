#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "neural_network/neural_network.h"

#ifndef UNIT_TEST

int main(int argc, char **argv)
{
    if (argc != 3)
        errx(1, "Wrong number of arguments.");

    double a, b;
    if (strcmp(argv[1], "1") == 0)
        a = 1.0;
    else if (strcmp(argv[1], "0") == 0)
        a = 0.0;
    else
        errx(1, "Parameter a at position 1 is neither 0 not 1.");

    if (strcmp(argv[2], "1") == 0)
        b = 1.0;
    else if (strcmp(argv[2], "0") == 0)
        b = 0.0;
    else
        errx(1, "Parameter b at position 1 is neither 0 not 1.");

    Neural_Network *net = net_load_from_file("xor.net");

    Matrix *input = mat_create_from_arr(2, 1, (double[]){a, b});
    Matrix *output = net_feed_forward(net, input, NULL, NULL);
    mat_print(output, 3);

    mat_free(input);
    mat_free(output);
    net_free(net);
}

#endif
