#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "neural_network.h"
#include "utils/math/sigmoid.h"
#include "utils/random/shuffle_array.h"

/// @brief Represents a fully connected neural network.
struct Neural_Network
{
    /// @brief Number of layers in the network (must be at least 2).
    size_t layer_number;
    /// @brief Array of layer heights (number of neurons per layer).
    size_t *layer_heights;
    /// @brief Array of bias matrices, one per layer (first element is NULL).
    Matrix **biases;
    /// @brief Array of weight matrices, one per layer (first element is NULL).
    Matrix **weights;
};

size_t net_layer_number(const Neural_Network *net) { return net->layer_number; }

size_t net_layer_height(const Neural_Network *net, size_t layer_id)
{
    if (layer_id >= net->layer_number)
        errx(EXIT_FAILURE, "Layer does not exist"); // todo

    return *(net->layer_heights + layer_id);
}

Neural_Network *net_create_empty(size_t layer_number, size_t *layer_heights)
{
    // Check if arguments are valid.
    if (layer_number < 2)
        errx(EXIT_FAILURE, "TODO");

    // Allocations.
    Neural_Network *net = malloc(sizeof(Neural_Network));
    if (net == NULL)
    {
        errx(EXIT_FAILURE, "TODO");
    }
    net->layer_number = layer_number;
    net->layer_heights = calloc(layer_number, sizeof(size_t));
    if (net->layer_heights == NULL)
    {
        free(net);
        errx(EXIT_FAILURE, "TODO");
    }
    net->weights = calloc(layer_number, sizeof(Matrix *));
    if (net->weights == NULL)
    {
        free(net->layer_heights);
        free(net);
        errx(EXIT_FAILURE, "TODO");
    }
    net->biases = calloc(layer_number, sizeof(Matrix *));
    if (net->biases == NULL)
    {
        free(net->layer_heights);
        free(net->weights);
        free(net);
        errx(EXIT_FAILURE, "TODO");
    }

    for (size_t i = 0; i < layer_number; i++)
        net->layer_heights[i] = layer_heights[i];

    net->weights[0] = NULL;
    net->biases[0] = NULL;
    for (size_t i = 1; i < layer_number; i++)
    {
        net->weights[i] = mat_create_uniform_random(
            layer_heights[i], layer_heights[i - 1], -1.0, 1.0);
        net->biases[i] =
            mat_create_uniform_random(layer_heights[i], 1, -1.0, 1.0);
    }

    return net;
}

void net_free(Neural_Network *net)
{
    free(net->layer_heights);
    mat_free_matrix_array(net->weights, net->layer_number);
    mat_free_matrix_array(net->biases, net->layer_number);
    free(net);
}

Neural_Network *net_load_from_file(char *filename)
{
    FILE *file_stream = fopen(filename, "r");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file: %s", filename);

    int fd = fileno(file_stream);
    if (fd == -1)
        errx(EXIT_FAILURE, "Failed to open file descriptor of file %s.",
             filename);

    Neural_Network *net = malloc(sizeof(Neural_Network));
    if (net == NULL)
        errx(EXIT_FAILURE,
             "Failed to allocate memory for net_load_from_file (net).");

    // The number of bytes read has read.
    ssize_t r_out;

    // Read the layer number.
    r_out = read(fd, &net->layer_number, sizeof(size_t));
    if (r_out != sizeof(size_t))
        errx(EXIT_FAILURE, "Invalid file %s: failed to read layer_number.",
             filename);

    // Allocate the necessary arrays.
    net->layer_heights = calloc(net->layer_number, sizeof(size_t));
    if (net->layer_heights == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for net_load_from_file "
                           "(net->layer_heights).");

    net->weights = calloc(net->layer_number, sizeof(Matrix *));
    if (net->weights == NULL)
        errx(
            1,
            "Failed to allocate memory for net_load_from_file (net->weights).");

    net->biases = calloc(net->layer_number, sizeof(Matrix *));
    if (net->biases == NULL)
        errx(EXIT_FAILURE,
             "Failed to allocate memory for net_load_from_file (net->biases).");

    // Read the height of the layers.
    for (size_t i = 0; i < net->layer_number; i++)
    {
        r_out = read(fd, &net->layer_heights[i], sizeof(size_t));
        if (r_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Invalid file %s: failed to read %zuth layer's height.",
                 filename, i);
    }

    // Read the weights.
    net->weights[0] = NULL;
    for (size_t i = 1; i < net->layer_number; i++)
    {
        size_t height, width;

        r_out = read(fd, &height, sizeof(size_t));
        if (r_out != sizeof(size_t))
            errx(
                1,
                "Invalid file %s: failed to read %zuth weight matrix's height.",
                filename, i);

        r_out = read(fd, &width, sizeof(size_t));
        if (r_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Invalid file %s: failed to read %zuth weight matrix's width.",
                 filename, i);

        float *content = calloc(height * width, sizeof(float));
        if (content == NULL)
            errx(EXIT_FAILURE,
                 "Failed to allocate memory for net_load_from_file (content).");

        // Read the matrix content.
        for (size_t j = 0; j < height * width; j++)
        {
            r_out = read(fd, &content[j], sizeof(float));
            if (r_out != sizeof(float))
                errx(EXIT_FAILURE,
                     "Invalid file %s: failed to read %zuth weight matrix's "
                     "%zuth coefficient.",
                     filename, i, j);
        }

        net->weights[i] = mat_create_from_arr(height, width, content);
        free(content);
    }

    // Read the biases.
    net->biases[0] = NULL;
    for (size_t i = 1; i < net->layer_number; i++)
    {
        size_t height, width;

        r_out = read(fd, &height, sizeof(size_t));
        if (r_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Invalid file %s: failed to read %zuth bias matrix's height.",
                 filename, i);

        r_out = read(fd, &width, sizeof(size_t));
        if (r_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Invalid file %s: failed to read %zuth bias matrix's width.",
                 filename, i);

        float *content = calloc(height * width, sizeof(float));
        if (content == NULL)
            errx(EXIT_FAILURE,
                 "Failed to allocate memory for net_load_from_file (content).");

        // Read the matrix content.
        for (size_t j = 0; j < height * width; j++)
        {
            r_out = read(fd, &content[j], sizeof(float));
            if (r_out != sizeof(float))
                errx(EXIT_FAILURE,
                     "Invalid file %s: failed to read %zuth bias matrix's "
                     "%zuth coefficient.",
                     filename, i, j);
        }

        net->biases[i] = mat_create_from_arr(height, width, content);
        free(content);
    }

    fclose(file_stream);

    return net;
}

void net_save_to_file(const Neural_Network *net, char *filename)
{
    FILE *file_stream = fopen(filename, "w");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file: %s", filename);

    int fd = fileno(file_stream);
    if (fd == -1)
        errx(EXIT_FAILURE, "Failed to open file descriptor of file %s.",
             filename);

    // The number of bytes write has written.
    ssize_t w_out;

    // Write the layer number.
    w_out = write(fd, &net->layer_number, sizeof(size_t));
    if (w_out != sizeof(size_t))
        errx(EXIT_FAILURE,
             "Failed to write file %s: failed to write net->layer_number.",
             filename);

    // Write the height of the layers.
    for (size_t i = 0; i < net->layer_number; i++)
    {
        w_out = write(fd, &net->layer_heights[i], sizeof(size_t));
        if (w_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Failed to write file %s: failed to write %zuth layer's "
                 "height.",
                 filename, i);
    }

    // Write the weights.
    for (size_t i = 1; i < net->layer_number; i++)
    {
        size_t write_buff;

        write_buff = mat_height(net->weights[i]);
        w_out = write(fd, &write_buff, sizeof(size_t));
        if (w_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Failed to write file %s: failed to write %zuth weight "
                 "matrix's height.",
                 filename, i);

        write_buff = mat_width(net->weights[i]);
        w_out = write(fd, &write_buff, sizeof(size_t));
        if (w_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Failed to write file %s: failed to write %zuth weight "
                 "matrix's width.",
                 filename, i);

        // Write the matrix content.
        for (size_t h = 0; h < mat_height(net->weights[i]); h++)
        {
            for (size_t w = 0; w < mat_width(net->weights[i]); w++)
            {
                w_out = write(fd, mat_coef_ptr(net->weights[i], h, w),
                              sizeof(float));
                if (w_out != sizeof(float))
                    errx(
                        1,
                        "Failed to write file %s: failed to write %zuth weight "
                        "matrix's coefficient at position (h:%zu, w:%zu).",
                        filename, i, h, w);
            }
        }
    }

    // Write the biases.
    for (size_t i = 1; i < net->layer_number; i++)
    {
        size_t write_buff;

        write_buff = mat_height(net->biases[i]);
        w_out = write(fd, &write_buff, sizeof(size_t));
        if (w_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Failed to write file %s: failed to write %zuth bias matrix's "
                 "height.",
                 filename, i);

        write_buff = mat_width(net->biases[i]);
        w_out = write(fd, &write_buff, sizeof(size_t));
        if (w_out != sizeof(size_t))
            errx(EXIT_FAILURE,
                 "Failed to write file %s: failed to write %zuth bias matrix's "
                 "width.",
                 filename, i);

        // Write the matrix content.
        for (size_t h = 0; h < mat_height(net->biases[i]); h++)
        {
            for (size_t w = 0; w < mat_width(net->biases[i]); w++)
            {
                w_out = write(fd, mat_coef_ptr(net->biases[i], h, w),
                              sizeof(float));
                if (w_out != sizeof(float))
                    errx(EXIT_FAILURE,
                         "Failed to write file %s: failed to write %zuth bias "
                         "matrix's coefficient at position (h:%zu, w:%zu).",
                         filename, i, h, w);
            }
        }
    }

    fclose(file_stream);
}

Matrix *net_feed_forward(const Neural_Network *net, Matrix *input,
                         Matrix *layers_results[net_layer_number(net)],
                         Matrix *layers_activations[net_layer_number(net)])
{
    if (mat_height(input) != net->layer_heights[0])
        errx(EXIT_FAILURE, "TODO");
    if (mat_width(input) != 1)
        errx(EXIT_FAILURE, "TODO");
    if ((layers_results == NULL) != (layers_activations == NULL))
    {
        errx(EXIT_FAILURE, "TODO");
    }

    // Whether layers_results and layers_activations should be filled.
    int out_params = layers_results != NULL;
    if (out_params)
    {
        layers_results[0] = NULL;
        layers_activations[0] = mat_deepcopy(input);
    }

    Matrix *prev_activation = mat_deepcopy(input);

    for (size_t i = 1; i < net->layer_number; i++)
    {
        // Compute the result between layer i and i + 1.

        // The activation vector (column matrix) after the ith layer.
        Matrix *curr_activation =
            mat_multiplication(net->weights[i], prev_activation);
        mat_inplace_addition(curr_activation, net->biases[i]);

        if (out_params)
        {
            // Store the result column matrix before the application of the
            // activation function.
            layers_results[i] = mat_deepcopy(curr_activation);

            mat_inplace_sigmoid(curr_activation);

            // Store the activation column matrix.
            layers_activations[i] = mat_deepcopy(curr_activation);
        }
        else
        {
            mat_inplace_sigmoid(curr_activation);
        }

        mat_free(prev_activation);
        prev_activation = curr_activation;
    }

    /*
    // Makes the result a probability distribution: the sum of the coefficient
    // of all classes is equal to 1. The result of the neural network can
    // therefore be seen as probabilities rather than activation coefficients.
    // Avoids to normalize a bit output (matrix of 1 by 1) which would always
    // result in 1 as output.
    if (mat_height(prev_activation) > 1 || mat_width(prev_activation) > 1)
        mat_inplace_normalize(prev_activation);
    */

    return prev_activation;
}

void net_back_propagation(Neural_Network *net, Matrix *expected,
                          Matrix *layers_results[net_layer_number(net)],
                          Matrix *layers_activations[net_layer_number(net)],
                          Matrix *delta_nabla_w[net_layer_number(net)],
                          Matrix *delta_nabla_b[net_layer_number(net)])
{
    if (delta_nabla_w == NULL)
        errx(EXIT_FAILURE, "TODO");
    if (delta_nabla_b == NULL)
        errx(EXIT_FAILURE, "TODO");

    // The error column matrix for the currently studied layer.
    Matrix *delta;
    // Temporary matrices.
    Matrix *a, *b;

    // Compute the error column matrix for the last layer.

    // delta = (act[L - 1] - expected) ⊙ σ'(res[L - 1])
    a = mat_subtraction(layers_activations[net->layer_number - 1], expected);
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

    // Compute the error column matrix for each layer i starting from the
    // penultimate layer to the first one (excluding the input layer).
    for (size_t i = net->layer_number - 2; i > 0; i--)
    {
        // delta = ((net.weights[i + 1])^T × delta) ⊙ σ'(res[i])
        a = mat_transpose(net->weights[i + 1]);
        b = mat_multiplication(a, delta);
        mat_free(a);
        a = b;
        b = mat_sigmoid_derivative(layers_results[i]);
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
                size_t batch_size, float learning_rate)
{
    for (size_t i = 1; i < net->layer_number; i++)
    {
        mat_inplace_scalar_multiplication(nabla_w[i],
                                          learning_rate / (float)batch_size);
        mat_inplace_scalar_multiplication(nabla_b[i],
                                          learning_rate / (float)batch_size);

        mat_inplace_subtraction(net->weights[i], nabla_w[i]);
        mat_inplace_subtraction(net->biases[i], nabla_b[i]);
    }
}

void net_train(Neural_Network *net, Training_Data **training_data,
               size_t training_data_size, size_t epochs, size_t batch_size,
               float learning_rate)
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
                nabla_w[i] = mat_create_zero(mat_height(net->weights[i]),
                                             mat_width(net->weights[i]));
                nabla_b[i] = mat_create_zero(mat_height(net->biases[i]),
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

                for (size_t j = 1; j < net->layer_number; j++)
                {
                    mat_inplace_addition(nabla_w[j], delta_nabla_w[j]);
                    mat_inplace_addition(nabla_b[j], delta_nabla_b[j]);
                }

                mat_free_matrix_array(layers_results, net->layer_number);
                mat_free_matrix_array(layers_activations, net->layer_number);
                mat_free_matrix_array(delta_nabla_w, net->layer_number);
                mat_free_matrix_array(delta_nabla_b, net->layer_number);
            }

            net_update(net, nabla_w, nabla_b, batch_size, learning_rate);

            mat_free_matrix_array(nabla_w, net->layer_number);
            mat_free_matrix_array(nabla_b, net->layer_number);
        }

        if ((epoch + 1) % 100 == 0)
        {
            size_t successes = 0;
            for (size_t i = 0; i < training_data_size; i++)
            {
                Matrix *output =
                    net_feed_forward(net, training_data[i]->input, NULL, NULL);
                mat_inplace_map(output, roundf);
                if (mat_eq(output, training_data[i]->expected))
                    successes++;
                mat_free(output);
            }
            printf("Epoch %zu / %zu completed with accuracy %.2lf%% (%zu / "
                   "%zu).\n",
                   epoch + 1, epochs,
                   100.0 * (double)successes / (double)training_data_size,
                   successes, training_data_size);
        }
    }
}

void net_print(Neural_Network *net, unsigned int precision)
{
    printf("/---------[WEIGHTS]----------\n");
    for (size_t i = 1; i < net->layer_number; i++)
    {
        mat_print(net->weights[i], precision);
        if (i != net->layer_number - 1)
            printf("\n");
    }
    printf("\\---------[WEIGHTS]----------\n");
    printf("/---------[BIASES ]----------\n");
    for (size_t i = 1; i < net->layer_number; i++)
    {
        mat_print(net->biases[i], precision);
        if (i != net->layer_number - 1)
            printf("\n");
    }
    printf("\\---------[BIASES ]----------\n");
}
