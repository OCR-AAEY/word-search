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

#define EPOCH_STEP 1

int main()
{
    rand_seed();

    char filename[1024];

    Dataset *original = ds_load_from_compressed_file(
        "./assets/dataset/real_compressed.dataset");

    Dataset *ds_train, *ds_test;
    ds_split(original, 0.20f, &ds_train, &ds_test);

    Neural_Network *net = net_create_empty(3, (size_t[]){784, 128, 26});

    float accuracy = 0.0f;
    size_t epochs = 0;

    while (accuracy < 0.90f)
    {
        // Train.
        net_train(net, ds_train, EPOCH_STEP, 128, 0.01);
        epochs += EPOCH_STEP;

        // Measure accuracy.
        size_t successes = 0;
        for (size_t i = 0; i < ds_size(ds_test); i++)
        {
            Training_Data *td = ds_get_data(ds_test, i);
            Matrix *output = net_feed_forward(net, td->input, NULL, NULL);
            if (mat_max_h(output) == td->expected_class)
                successes++;
            mat_free(output);
        }
        accuracy = (float)successes / (float)ds_size(ds_test);

        // Print info into stdout.
        time_t currentTime = time(NULL);
        char *time_str = ctime(&currentTime);
        time_str[strlen(time_str) - 1] = '\0';
        printf("[%s] Epoch %zu completed with accuracy %.2lf%% (%zu / %zu).\n",
               time_str, epochs, 100.0f * accuracy, successes,
               ds_size(ds_test));
        fflush(stdout);

        snprintf(filename, 1024, "ocr_real_%.3f.nn", accuracy);
        net_save_to_file(net, filename);
    }

    net_save_to_file(net, "ocr_real_final.nn");

    net_free(net);

    ds_free(ds_train);
    ds_free(ds_test);
}
