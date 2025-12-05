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

Training_Data *td_create(Matrix *input, size_t expected_class);

void td_free(Training_Data *td);

size_t ds_size(Dataset *dataset);

Dataset *ds_create_empty();

void ds_add_tuple(Dataset *dataset, Training_Data *tuple);

Dataset *ds_load_from_directory(char *dirname);

Dataset *ds_load_from_nested_directory(char *dirname);

Dataset *ds_load_from_file(char *filename);

void ds_save_to_file(Dataset *ds, char *filename);

Dataset *ds_load_from_compressed_file(char *filename);

void ds_save_to_compressed_file(Dataset *ds, const char *filename);

void ds_shuffle(Dataset *dataset);

/// @brief
/// @param dataset This dataset is freed after the function call.
/// @param test_percentage
/// @param out_train
/// @param out_test
void ds_split(Dataset *dataset, float test_percentage, Dataset **out_train,
              Dataset **out_test);

Training_Data *ds_get_data(Dataset *dataset, size_t i);

void ds_free(Dataset *dataset);

#endif
