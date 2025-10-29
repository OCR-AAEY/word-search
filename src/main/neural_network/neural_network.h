#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "matrix/matrix.h"
#include <stddef.h>

/// @brief Represents a fully connected neural network.
typedef struct Neural_Network Neural_Network;

/// @brief Represents a single training example for a neural network.
struct Training_Data
{
    /// @brief Input column matrix for the network.
    Matrix *input;
    /// @brief Expected output column matrix corresponding to the input.
    Matrix *expected;
};

/// @brief Represents a single training example for a neural network.
typedef struct Training_Data Training_Data;

/// @brief Retrieves the number of layers in a neural network.
/// @param[in] net Pointer to the Neural_Network structure.
/// @return The number of layers in the network.
size_t net_layer_number(const Neural_Network *net);

/// @brief Retrieves the height (number of neurons) of a specific layer in a
/// neural network.
/// @param[in] net Pointer to the Neural_Network structure.
/// @param[in] layer_id Index of the layer to query (0-based).
/// @return The number of neurons in the specified layer.
/// @throw Exits the program with an error if the specified layer does not
/// exist.
size_t net_layer_height(const Neural_Network *net, size_t layer_id);

/// @brief Creates a new neural network with randomly initialized weights and
/// biases.
/// @param[in] layer_number Number of layers in the network (must be at least
/// 2).
/// @param[in] layer_heights Array specifying the number of neurons in each
/// layer. This array is copied and should therefore be freed manually if
/// necessary.
/// @return Pointer to a newly allocated Neural_Network structure.
/// @throw Exits the program with an error if memory allocation fails or if
/// layer_number < 2.
Neural_Network *net_create_empty(size_t layer_number, size_t *layer_heights);

/// @brief Frees all memory associated with a neural network.
/// @param[in, out] net Pointer to the Neural_Network to be freed.
/// @note The layer_heights array is also freed by this function.
void net_free(Neural_Network *net);

/// @brief Loads a neural network from a binary file.
/// @param[in] filename Path to the file containing the serialized neural
/// network.
/// @return Pointer to a newly allocated Neural_Network structure with weights
/// and biases loaded from the file.
/// @throws Exits the program if the file cannot be opened, memory allocation
/// fails, or the file contents are invalid.
Neural_Network *net_load_from_file(char *filename);

/// @brief Saves a neural network to a binary file.
/// @param[in] net Pointer to the Neural_Network to be saved.
/// @param[in] filename Path to the file where the network will be written.
/// @throw Exits the program if the file cannot be opened or if writing fails at
/// any point.
void net_save_to_file(const Neural_Network *net, char *filename);

/// @brief Computes the forward pass of a neural network on a given input.
/// @param[in] net Pointer to the Neural_Network to use for the forward pass.
/// @param[in] input Column matrix representing the input; it is not freed by
/// this function.
/// @param[out] layers_results Optional array to store the pre-activation
/// results for each layer (can be NULL). Note that the first element is always
/// NULL because the input itself is the first post-activation column matrix and
/// therefore the first pre-activation column matrix is unknown.
/// @param[out] layers_activations Optional array to store the post-activation
/// values for each layer (can be NULL).
/// @return A newly allocated, normalized column matrix representing the network
/// output (deep copy of the last layer's activation).
/// @throw Exits the program if the input dimensions do not match the network's
/// input layer, or if only one of layers_results or layers_activations is
/// provided (must be both or neither).
Matrix *net_feed_forward(const Neural_Network *net, Matrix *input,
                         Matrix *layers_results[net_layer_number(net)],
                         Matrix *layers_activations[net_layer_number(net)]);

/// @brief Performs backpropagation on a neural network, computing the gradients
/// of the cost function.
/// @param[in] net Pointer to the Neural_Network to update.
/// @param[in] expected Column matrix of expected output values for the current
/// input.
/// @param[in] layers_results Array of pre-activation results for each layer.
/// @param[in] layers_activations Array of post-activation values for each
/// layer.
/// @param[out] delta_nabla_w Array to store the gradients of the weights for
/// each layer. The first element of delta_nabla_w is always NULL.
/// @param[out] delta_nabla_b Array to store the gradients of the biases for
/// each layer. The first element of delta_nabla_w is always NULL.
/// @throw Exits the program if delta_nabla_w or delta_nabla_b arrays are NULL.
void net_back_propagation(Neural_Network *net, Matrix *expected,
                          Matrix *layers_results[net_layer_number(net)],
                          Matrix *layers_activations[net_layer_number(net)],
                          Matrix *delta_nabla_w[net_layer_number(net)],
                          Matrix *delta_nabla_b[net_layer_number(net)]);

/// @brief Updates a neural network's weights and biases using computed
/// gradients and a learning rate.
/// @param[in, out] net Pointer to the Neural_Network to update.
/// @param[in] nabla_w Array of weight gradients for each layer.
/// @param[in] nabla_b Array of bias gradients for each layer.
/// @param[in] learning_rate Scalar value to scale the gradients during the
/// update.
/// @throw None. Assumes all matrices are properly allocated.
void net_update(Neural_Network *net, Matrix **nabla_w, Matrix **nabla_b,
                size_t batch_size, double learning_rate);

/// @brief Trains a neural network using mini-batch stochastic gradient descent.
/// @param[in, out] net Pointer to the Neural_Network to train; its weights and
/// biases are updated.
/// @param[in] training_data Array of pointers to Training_Data containing
/// input-output pairs.
/// @param[in] training_data_size Number of training samples in the array.
/// @param[in] epochs Number of times to iterate over the entire training
/// dataset.
/// @param[in] batch_size Number of samples per mini-batch.
/// @param[in] learning_rate Scalar to scale the gradient updates.
/// @throw Exits the program if any memory allocation fails during training.
void net_train(Neural_Network *net, Training_Data **training_data,
               size_t training_data_size, size_t epochs, size_t batch_size,
               double learning_rate);

/// @brief Prints all weights and biases of the given neural network. Displays
/// each weight and bias matrix in a readable format using `mat_print()`. The
/// output is grouped and labeled for clarity.
/// @param[in] net Pointer to the neural network to print. Must not be NULL.
/// @param[in] precision The number of decimal digits to display for each matrix
/// element.
/// @return void
/// @throw Terminates the program if `net` or any of its matrices are invalid.
/// @note This function is intended for debugging or inspection purposes only.
/// The printed format is human-readable, not machine-parsable.
void net_print(Neural_Network *net, unsigned int precision);

#endif
