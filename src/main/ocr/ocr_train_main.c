#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "dataset.h"
#include "matrix/matrix.h"
#include "neural_network.h"
#include "utils/random/random.h"

#define EPOCH_STEP 20

/* Train on real dataset.

void print_info(Neural_Network *net, size_t epoch, Dataset *ds_test,
                Dataset *ds_grid)
{
    // Time info
    time_t currentTime = time(NULL);
    char *time_str = ctime(&currentTime);
    time_str[strlen(time_str) - 1] = '\0';
    printf("[%s] Epoch %zu completed with:\n", time_str, epoch);

    size_t successes;
    float accuracy;

    // Real dataset info
    successes = 0;
    for (size_t i = 0; i < ds_size(ds_test); i++)
    {
        Training_Data *td = ds_get_data(ds_test, i);
        Matrix *output = net_feed_forward(net, td->input, NULL, NULL);
        if (mat_max_h(output) == td->expected_class)
            successes++;
        mat_free(output);
    }
    accuracy = (float)successes / (float)ds_size(ds_test);
    printf("                           TEST: Accuracy %.2lf%% (%zu / %zu).\n",
           100.0f * accuracy, successes, ds_size(ds_test));

    // Grid dataset info
    successes = 0;
    for (size_t i = 0; i < ds_size(ds_grid); i++)
    {
        Training_Data *td = ds_get_data(ds_grid, i);
        Matrix *output = net_feed_forward(net, td->input, NULL, NULL);
        if (mat_max_h(output) == td->expected_class)
            successes++;
        mat_free(output);
    }
    accuracy = (float)successes / (float)ds_size(ds_grid);
    printf("                           GRID: Accuracy %.2lf%% (%zu / %zu).\n",
           100.0f * accuracy, successes, ds_size(ds_grid));

    fflush(stdout);
}

int main()
{
    rand_seed();

    Dataset *ds_grid = ds_load_from_compressed_file(
        "./assets/dataset/grid.dataset");

    Dataset *ds_real = ds_load_from_compressed_file(
        "./assets/dataset/real.dataset");

    Dataset *ds_train, *ds_test;
    ds_split(ds_real, 0.20f, &ds_train, &ds_test);

    Neural_Network *net = net_create_empty(3, (size_t[]){784, 128, 26});

    size_t epoch = 0;

    print_info(net, epoch, ds_test, ds_grid);

    while (1)
    {
        net_train(net, ds_train, EPOCH_STEP, 128, 0.01);
        epoch += EPOCH_STEP;

        print_info(net, epoch, ds_test, ds_grid);

        net_save_to_file(net, "ocr_real.nn");
    }

    net_free(net);

    ds_free(ds_train);
    ds_free(ds_test);
    ds_free(ds_grid);
}

*/

float print_info(Neural_Network *net, size_t epoch, Dataset *ds_test)
{
    // Time info
    time_t currentTime = time(NULL);
    char *time_str = ctime(&currentTime);
    time_str[strlen(time_str) - 1] = '\0';
    printf("[%s] Epoch %zu completed with:\n", time_str, epoch);

    size_t successes = 0;
    for (size_t i = 0; i < ds_size(ds_test); i++)
    {
        Training_Data *td = ds_get_data(ds_test, i);
        Matrix *output = net_feed_forward(net, td->input, NULL, NULL);
        if (mat_max_h(output) == td->expected_class)
            successes++;
        mat_free(output);
    }
    float accuracy = (float)successes / (float)ds_size(ds_test);
    printf("                           Accuracy %.2lf%% (%zu / %zu).\n",
           100.0f * accuracy, successes, ds_size(ds_test));

    fflush(stdout);

    return accuracy;
}

int main()
{
    rand_seed();

    Dataset *ds_grid =
        ds_load_from_compressed_file("./assets/ocr/dataset/grid.dataset");

    Dataset *ds_train, *ds_test;
    ds_split(ds_grid, 0.20f, &ds_train, &ds_test);

    Neural_Network *net = net_create_empty(3, (size_t[]){784, 128, 26});

    size_t epoch = 0;

    float accuracy = print_info(net, epoch, ds_test);

    while (accuracy < 0.90f)
    {
        net_train(net, ds_train, EPOCH_STEP, 64, 0.01);
        epoch += EPOCH_STEP;

        accuracy = print_info(net, epoch, ds_test);
    
        net_save_to_file(net, "ocr_grid.nn");
    }

    net_free(net);

    ds_free(ds_train);
    ds_free(ds_test);
}