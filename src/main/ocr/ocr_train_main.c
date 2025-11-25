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
/* PROD
int main()
{
    Dataset *dataset =
        ds_load_from_directory("./assets/ocr-training-dataset/matrices/");

    Neural_Network *net = net_create_empty(3, (size_t[]){784, 128, 26});

    float accuracy = 0.0f;
    size_t epochs = 1;
    while (accuracy < 0.5f)
    {
        net_train(net, dataset, 10000, 64, 0.01);

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

        time_t currentTime;
        time(&currentTime);

        printf("%s    Epoch %zu completed with accuracy %.2lf%% (%zu / %zu).\n",
               ctime(&currentTime), 10000 * epochs, 100.0f * accuracy,
               successes, ds_size(dataset));
        fflush(stdout);
        char filename[1024];
        sprintf(filename, "ocr-%zuk.nn", epochs);
        net_save_to_file(net, filename);

        epochs++;
    }

    net_save_to_file(net, "ocr.nn");

    net_free(net);

    ds_free(dataset);
}
*/

int main(void)
{
    Dataset *dataset =
        ds_load_from_directory("./assets/ocr-training-dataset/matrices/");

    Neural_Network *net = net_create_empty(3, (size_t[]){784, 128, 26});

    float accuracy = 0.0f;
    size_t epochs = 1;
    while (accuracy < 0.9f)
    {
        net_train(net, dataset, 1, 1, 0.01f);

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

        time_t currentTime;
        time(&currentTime);

        printf("%s    Epoch %zu completed with accuracy %.2lf%% (%zu / %zu).\n",
               ctime(&currentTime), 1 * epochs, 100.0f * accuracy,
               successes, ds_size(dataset));
        fflush(stdout);

        epochs++;
    }

    net_free(net);

    ds_free(dataset);
}