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

#define EPOCH_STEP 10

int main()
{
    rand_seed();

    char filename[1024];

    Dataset *dataset =
        ds_load_from_directory("./assets/ocr_dataset/real/matrices/");

    Neural_Network *net = net_create_empty(3, (size_t[]){784, 128, 26});

    float accuracy = 0.0f;
    size_t epochs = 0;

    while (accuracy < 0.90f)
    {
        // Train.
        net_train(net, dataset, EPOCH_STEP, 64, 0.01);
        epochs += EPOCH_STEP;

        // Measure accuracy.
        size_t successes = 0;
        for (size_t i = 0; i < ds_size(dataset); i++)
        {
            Training_Data *td = ds_get_data(dataset, i);
            Matrix *output = net_feed_forward(net, td->input, NULL, NULL);
            if (mat_max_h(output) == td->expected_class)
                successes++;
            mat_free(output);
        }
        accuracy = (float)successes / (float)ds_size(dataset);

        // Print info into stdout.
        time_t currentTime = time(NULL);
        char *time_str = ctime(&currentTime);
        time_str[strlen(time_str) - 1] = '\0';
        printf("[%s] Epoch %zu completed with accuracy %.2lf%% (%zu / %zu).\n",
               time_str, epochs, 100.0f * accuracy, successes,
               ds_size(dataset));
        fflush(stdout);

        snprintf(filename, 1024, "ocr_real_%.3f.nn", accuracy);
        net_save_to_file(net, filename);
    }

    net_save_to_file(net, "ocr_real_final.nn");

    net_free(net);

    ds_free(dataset);
}
