#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix/matrix.h"
#include "neural_network.h"
#include "utils/shuffle_array.h"

struct Neural_Network
{
    /// @brief At least 2
    unsigned int layer_number;
    unsigned int *layer_heights;
    /// @brief Array of matrix pointers (first is NULL)
    Matrix **biases;
    /// @brief Array of matrix pointers (first is NULL)
    Matrix **weights;
};

unsigned int net_layer_number(const Neural_Network *net)
{
    return net->layer_number;
}

unsigned int net_layer_height(const Neural_Network *net, unsigned int layer_id)
{
    if (layer_id >= net->layer_number)
        errx(1, "Layer does not exist"); // todo

    return *(net->layer_heights + layer_id);
}

// Allocates mem. Do not free layer_heights. Random weights.
Neural_Network *net_create_empty(unsigned int layer_number,
                                 unsigned int *layer_heights)
{
    Neural_Network *net = malloc(sizeof(Neural_Network));

    if (net == NULL)
        errx(1, "Failed to allocate memory with malloc.");

    *net = (Neural_Network){.layer_number = layer_number,
                            .layer_heights = layer_heights,
                            .biases = NULL,   // todo
                            .weights = NULL}; // todo

    return net;
}

Neural_Network *net_load_from_file(char *filename)
{
    // todo
}

void net_save_to_file(Neural_Network *net, char *filename)
{
    // todo
}

/// @brief The result of input through the net.
/// @param net
/// @param input
/// @return
Matrix *feed_forward(Neural_Network *net, Matrix *input)
{
    if (height(input) != 1)
        errx(1, "invalid dimension"); // todo
    if (width(input) != *(net->layer_heights))
        errx(1, "invalid dimension"); // todo

    Matrix *mat = input;
    Matrix *tmp;

    for (int i = 0; i < net->layer_number - 1; i++)
    {
        // computing result between layer i and i+1
        tmp = mat_multiplication(mat, *(net->weights + i));
        mat_free(mat);
        mat = tmp;

        tmp = mat_addition(mat, *(net->biases + i));
        mat_free(mat);
        mat = tmp;

        tmp = mat_sigmoid(mat);
        free(mat);
        mat = tmp;
    }

    mat_normalize(mat);

    return mat;
}

/// @brief
/// @param net
/// @param batch_inputs array of matrices representing the inputs
/// @param batch_outputs array of matrices representing the expected outputs
/// with respect to the input matrix in batch_inputs.
/// @param batch_size
/// @param learning_rate
void update_batch(Neural_Network *net, Matrix **batch_inputs,
                  Matrix **batch_outputs, unsigned long batch_size,
                  double learning_rate)
{

    Matrix **biases = calloc(net->layer_number, sizeof(Matrix *));
    for (size_t i = 1; i < net->layer_number; i++)
    {
        *(biases + i) = create_empty_matrix(*(net->layer_heights + i), 1);
    }
    Matrix **weights = calloc(net->layer_number, sizeof(Matrix *));
    for (size_t i = 1; i < net->layer_number; i++)
    {
        *(weights + i) = create_empty_matrix(*(net->layer_heights + i), 1);
    }

    for (size_t i = 0; i < batch_size; i++)
    {
        delta_nabla_b, delta_nabla_w = self.backprop(x, y)
        biases = [nb+dnb for nb, dnb in zip(biases, delta_nabla_b)]
        weights = [nw+dnw for nw, dnw in zip(weights, delta_nabla_w)]
    }

    self.weights = [w-(eta/len(mini_batch))*nw 
                    for w, nw in zip(self.weights, weights)]
    self.biases = [b-(eta/len(mini_batch))*nb 
                    for b, nb in zip(self.biases, biases)]
}

void stochastic_gradient_descent(Neural_Network *net, void **training_data,
                                 unsigned long training_data_size,
                                 unsigned int epochs, unsigned int batch_size,
                                 double learning_rate)
{
    for (unsigned int i = 0; i < epochs; i++)
    {
        shuffle_array(training_data, training_data_size);
        for (unsigned int j = 0; j < training_data_size / batch_size; j++)
        {
            net_update_batch(net, training_data + j, batch_size, learning_rate);
        }

        // todo manage test data

        printf("Epoch %u completed.\n", i);
    }
}
