#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix/matrix.h"
#include "neural_network.h"
#include "utils/math/sigmoid.h"
#include "utils/random/shuffle_array.h"

struct Neural_Network
{
    /// @brief At least 2
    size_t layer_number;
    size_t *layer_heights;
    /// @brief Array of matrix pointers (first is NULL)
    Matrix **biases;
    /// @brief Array of matrix pointers (first is NULL)
    Matrix **weights;
};

size_t net_layer_number(const Neural_Network *net) { return net->layer_number; }

size_t net_layer_height(const Neural_Network *net, size_t layer_id)
{
    if (layer_id >= net->layer_number)
        errx(1, "Layer does not exist"); // todo

    return *(net->layer_heights + layer_id);
}

/// @brief Allocates mem. Do not free layer_heights. Random weights.
/// @param layer_number
/// @param layer_heights
/// @return
Neural_Network *net_create_empty(size_t layer_number, size_t *layer_heights)
{
    // Check if arguments are valid.
    if (layer_number < 2)
        errx(1, "TODO");

    // Allocations.
    Neural_Network *net = malloc(sizeof(Neural_Network));
    if (net == NULL)
    {
        errx(1, "TODO");
    }
    Matrix **weights = calloc(layer_number, sizeof(Matrix *));
    if (weights == NULL)
    {
        free(net);
        errx(1, "TODO");
    }
    Matrix **biases = calloc(layer_number, sizeof(Matrix *));
    if (biases == NULL)
    {
        free(net);
        free(weights);
        errx(1, "TODO");
    }

    weights[0] = NULL;
    biases[0] = NULL;
    for (size_t i = 1; i < layer_number; i++)
    {
        weights[i] =
            mat_create_gaussian_random(layer_heights[i], layer_heights[i - 1]);
        biases[i] = mat_create_gaussian_random(layer_heights[i], 1);
    }

    *net = (Neural_Network){.layer_number = layer_number,
                            .layer_heights = layer_heights,
                            .biases = biases,
                            .weights = weights};

    return net;
}

void net_free(Neural_Network *net)
{
    free(net->layer_heights);
    for (size_t i = 1; net->layer_number; i++)
    {
        mat_free(net->weights[i]);
        mat_free(net->biases[i]);
    }
    free(net->weights);
    free(net->biases);
    free(net);
}

Neural_Network *net_load_from_file(char *filename)
{
    // TODO
}

void net_save_to_file(const Neural_Network *net, char *filename)
{
    // TODO
}

/// @brief The result of input through the net.
/// @param net
/// @param input is not freed
/// @param[out] layers_results
/// @param[out] layers_activations
/// @return A normalized matrix equal to activation[last] but not same (deep
/// copied).
Matrix *net_feed_forward(const Neural_Network *net, Matrix *input,
                         Matrix *layers_results[net->layer_number],
                         Matrix *layers_activations[net->layer_number])
{
    if (mat_height(input) != net->layer_heights[0])
        errx(1, "TODO");
    if (mat_width(input) != 1)
        errx(1, "TODO");
    if ((layers_results == NULL) != (layers_activations == NULL))
    {
        errx(1, "TODO");
    }

    int out_params = layers_results != NULL;
    if (out_params)
    {
        layers_results[0] = NULL;
        layers_activations[0] = NULL;
    }

    Matrix *prev_activation = mat_deepcopy(input);

    for (size_t i = 1; i < net->layer_number; i++)
    {
        // Computes the result between layer i and i + 1.

        // The activation vector (column matrix) after the ith layer.
        Matrix *curr_activation =
            mat_multiplication(net->weights[i], prev_activation);
        mat_inplace_addition(curr_activation, net->biases[i]);

        if (out_params)
        {
            layers_results[i] = mat_deepcopy(curr_activation);
            mat_inplace_sigmoid(curr_activation);
            layers_activations[i] = mat_deepcopy(curr_activation);
        }
        else
        {
            mat_inplace_sigmoid(curr_activation);
            mat_free(prev_activation);
        }

        prev_activation = curr_activation;
    }

    // Makes the result a probability distribution: the sum of the coefficient
    // of all classes is equal to 1. The result of the neural network can
    // therefore be seen as probabilities rather than activation coefficients.
    mat_normalize(prev_activation);

    return prev_activation;
}

void net_back_propagation(Neural_Network *net, Matrix *expected,
                          Matrix *layers_results[net->layer_number],
                          Matrix *layers_activations[net->layer_number],
                          Matrix *delta_nabla_w[net->layer_number],
                          Matrix *delta_nabla_b[net->layer_number])
{
    if (delta_nabla_w == NULL)
        errx(1, "TODO");
    if (delta_nabla_b == NULL)
        errx(1, "TODO");

    // The error column matrix for the currently studied layer.
    Matrix *delta;
    // Temporary matrices.
    Matrix *a, *b;

    // Compute the error column matrix for the last layer.

    // delta = (act[L - 1] - expected) ⊙ σ'(res[L - 1])
    a = mat_substraction(layers_activations[net->layer_number - 1], expected);
    b = mat_sigmoid_derivative(layers_results[net->layer_number - 1]);
    delta = mat_hadamard(a, b);
    mat_free(a);
    mat_free(b);

    // delta_nabla_w = delta × (act[L - 2])^T
    a = mat_transpose(layers_activations[net->layer_number - 2]);
    delta_nabla_w[net->layer_number - 1] = mat_multiplication(delta, a);
    mat_free(a);

    // delta_nabla_b = delta
    delta_nabla_b[net->layer_number - 1] = delta;

    // Compute the error column matrix for each layer starting from the
    // penultimate layer to the first one (excluding the input layer).
    for (size_t i = net->layer_number - 2; i >= 1; i--)
    {
        // delta = ((net.weights[i + 1])^T × delta) ⊙ σ'(res[i])
        a = mat_transpose(net->weights[i + 1]);
        b = mat_multiplication(a, delta);
        mat_free(a);
        a = b;
        b = mat_sigmoid_derivative(layers_results[i]);
        mat_free(delta);
        delta = mat_hadamard(a, b);
        mat_free(a);
        mat_free(b);

        // a = delta × (act[i - 1])^T
        a = mat_transpose(layers_activations[i - 1]);
        delta_nabla_w[i] = mat_multiplication(delta, a);
        mat_free(a);

        // delta_nabla_b = delta
        delta_nabla_b[i] = delta;
    }

    delta_nabla_w[0] = NULL;
    delta_nabla_b[0] = NULL;
}

void net_update(Neural_Network *net, Matrix **nabla_w, Matrix **nabla_b,
                double learning_rate)
{
    for (size_t i = 1; i < net->layer_number; i++)
    {
        mat_inplace_scalar_multiplication(nabla_w[i], learning_rate);
        mat_inplace_scalar_multiplication(nabla_b[i], learning_rate);

        mat_inplace_addition(net->weights[i], nabla_w[i]);
        mat_inplace_addition(net->biases[i], nabla_b[i]);
    }
}

void net_train(Neural_Network *net, Training_Data **training_data,
               size_t training_data_size, size_t epochs, size_t batch_size,
               double learning_rate)
{
    for (size_t epoch = 0; epoch < epochs; epoch++)
    {
        shuffle_array(training_data, sizeof(Training_Data *),
                      training_data_size);

        for (size_t batch = 0; batch < training_data_size / batch_size; batch++)
        {
            Matrix **nabla_w = calloc(net->layer_number, sizeof(Matrix *));
            Matrix **nabla_b = calloc(net->layer_number, sizeof(Matrix *));
            for (size_t i = 1; i < net->layer_number; i++)
            {
                nabla_w[i] = mat_create_empty(mat_height(net->weights[i]),
                                              mat_width(net->weights[i]));
                nabla_b[i] = mat_create_empty(mat_height(net->biases[i]),
                                              mat_width(net->biases[i]));
            }

            for (size_t i = 0; i < batch_size; i++)
            {
                Training_Data *data = training_data[batch * batch_size + i];
                Matrix **layers_results =
                    calloc(net->layer_number, sizeof(Matrix *));
                Matrix **layers_activations =
                    calloc(net->layer_number, sizeof(Matrix *));
                Matrix **delta_nabla_w =
                    calloc(net->layer_number, sizeof(Matrix *));
                Matrix **delta_nabla_b =
                    calloc(net->layer_number, sizeof(Matrix *));

                mat_free(net_feed_forward(net, data->input, layers_results,
                                          layers_activations));

                net_back_propagation(net, data->expected, layers_results,
                                     layers_activations, delta_nabla_w,
                                     delta_nabla_b);

                net_update(net, nabla_w, nabla_b, learning_rate);

                mat_free_matrix_array(layers_results, net->layer_number);
                mat_free_matrix_array(layers_activations, net->layer_number);
                mat_free_matrix_array(delta_nabla_w, net->layer_number);
                mat_free_matrix_array(delta_nabla_b, net->layer_number);
            }
        }
    }
}
