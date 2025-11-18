#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "dataset.h"
#include "matrix/matrix.h"
#include "neural_network.h"

int main()
{
    Dataset *dataset =
        ds_load_from_directory("./assets/ocr-training-dataset/matrices/");

    Neural_Network *net = net_load_from_file("ocr.net");

    net_train(net, dataset, 10, 64, 0.01);

    net_save_to_file(net, "ocr.net");

    net_free(net);

    ds_free(dataset);
}