#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "dataset.h"
#include "utils/random/shuffle_array.h"

struct Dataset
{
    Training_Data **content;
    size_t max_size;
    size_t size;
};

void td_free(Training_Data *td)
{
    free(td->input);
    free(td->expected);
    free(td);
}

size_t ds_size(Dataset *dataset) { return dataset->size; }

Dataset *ds_create_empty()
{
    Dataset *dataset = malloc(sizeof(Dataset));
    if (dataset == NULL)
        errx(EXIT_FAILURE, "failed to malloc");

    dataset->content = malloc(DATASET_SIZE_STEP * sizeof(Training_Data));
    if (dataset->content == NULL)
        errx(EXIT_FAILURE, "failed to malloc");

    dataset->max_size = DATASET_SIZE_STEP;
    dataset->size = 0;

    return dataset;
}

void ds_add_tuple(Dataset *dataset, Training_Data *tuple)
{
    if (dataset->size >= dataset->max_size)
    {
        dataset->max_size += DATASET_SIZE_STEP;
        Training_Data **ptr = realloc(dataset->content, dataset->max_size * sizeof(Training_Data));
        if (ptr == NULL)
            errx(EXIT_FAILURE, "failed to realloc");
        dataset->content = ptr;
    }

    // printf("%zu/%zu\n", dataset->size, dataset->max_size);
    dataset->content[dataset->size] = tuple;
    dataset->size++;
}

Dataset *ds_load_from_directory(char *dirname)
{
    Dataset *dataset = ds_create_empty();

    DIR *dir = opendir(dirname);
    if (dir == NULL)
        errx(EXIT_FAILURE, "failed to opendir");

    struct dirent *entry;
    char path[1024];

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        struct stat st;
        if (stat(path, &st) == -1)
            errx(EXIT_FAILURE, "failed to stat");

        if (!S_ISREG(st.st_mode))
            continue;

        Training_Data *tuple = malloc(sizeof(Training_Data));
        if (tuple == NULL)
            errx(EXIT_FAILURE, "failed to malloc");

        tuple->input = mat_load_from_file(path);
        mat_inplace_vertical_flatten(tuple->input);
        tuple->expected = mat_create_zero(26, 1);
        *mat_coef_ptr(tuple->expected, entry->d_name[0] - 'a', 0) = 1.0;

        ds_add_tuple(dataset, tuple);
    }

    closedir(dir);

    return dataset;
}

void ds_shuffle(Dataset *dataset)
{
    shuffle_array(dataset->content, sizeof(Training_Data *), dataset->size);
}

inline Training_Data *ds_get_data(Dataset *dataset, size_t i)
{
    return dataset->content[i];
}

void ds_free(Dataset *dataset)
{
    for (size_t i = 0; i < dataset->size; i++)
        td_free(dataset->content[i]);
    free(dataset);
}
