#ifndef DATASET_H
#define DATASET_H

#include <stddef.h>

#include "matrix/matrix.h"

#define DATASET_SIZE_STEP 128ul

struct Training_Data
{
    Matrix *input;
    Matrix *expected;
    size_t expected_class;
};

typedef struct Training_Data Training_Data;

typedef struct Dataset Dataset;

void td_free(Training_Data *td);

size_t ds_size(Dataset *dataset);

Dataset *ds_create_empty();

void ds_add_tuple(Dataset *dataset, Training_Data *tuple);

Dataset *ds_load_from_directory(char *dirname);

Dataset *ds_load_from_file(char *filename);

void ds_save_to_file(Dataset *ds, char *filename);

void ds_shuffle(Dataset *dataset);

Training_Data *ds_get_data(Dataset *dataset, size_t i);

void ds_free(Dataset *dataset);

#endif
