#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "neural_network/neural_network.h"

Training_Data **generate_training_data()
{
    Training_Data **training_data = calloc(4, sizeof(Training_Data *));
    if (training_data == NULL)
        errx(1, "Failed to allocate memory in generate_training_data.");

    for (size_t i = 0; i < 4; i++)
    {
        size_t a = (i >> 0) & 1;
        size_t b = (i >> 1) & 1;

        training_data[i] = malloc(sizeof(Training_Data));
        if (training_data[i] == NULL)
            errx(1, "Failed to allocate memory in generate_training_data.");

        training_data[i]->input =
            mat_create_from_arr(2, 1, (double[]){(double)a, (double)b});
        training_data[i]->expected =
            mat_create_from_arr(1, 1, (double[]){(double)!(a ^ b)});
    }

    return training_data;
}

void free_training_data(Training_Data **training_data)
{
    for (size_t i = 0; i < 4; i++)
    {
        mat_free(training_data[i]->input);
        mat_free(training_data[i]->expected);
        free(training_data[i]);
    }
    free(training_data);
}

#ifndef UNIT_TEST

int main()
{
    size_t layers[3] = {2, 2, 1};
    Neural_Network *net = net_create_empty(3, layers);
    Training_Data **training_data = generate_training_data();

    net_train(net, training_data, 4, 10000, 1, 1.0);

    net_save_to_file(net, "xor.net");

    free_training_data(training_data);
    net_free(net);
}

#endif
