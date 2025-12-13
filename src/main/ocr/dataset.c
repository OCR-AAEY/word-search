#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "dataset.h"
#include "utils/random/shuffle_array.h"

struct Dataset
{
    Training_Data **content;
    size_t max_size;
    size_t size;
};

Training_Data *td_create(Matrix *input, size_t expected_class)
{
    Training_Data *tuple = malloc(sizeof(Training_Data));
    if (tuple == NULL)
        errx(EXIT_FAILURE, "failed to malloc");

    tuple->input = input;
    tuple->expected = mat_create_zero(26, 1);
    *mat_coef_ptr(tuple->expected, expected_class, 0) = 1.0;
    tuple->expected_class = expected_class;

    return tuple;
}

void td_free(Training_Data *td)
{
    mat_free(td->input);
    mat_free(td->expected);
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
        Training_Data **ptr = realloc(
            dataset->content, dataset->max_size * sizeof(Training_Data));
        if (ptr == NULL)
            errx(EXIT_FAILURE, "failed to realloc");
        dataset->content = ptr;
    }

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

        Matrix *input = mat_load_from_file(path);
        mat_inplace_vertical_flatten(input);

        Training_Data *tuple = td_create(input, entry->d_name[0] - 'a');

        ds_add_tuple(dataset, tuple);
    }

    closedir(dir);

    return dataset;
}

Dataset *ds_load_from_nested_directory(char *dirname)
{
    Dataset *dataset = ds_create_empty();
    DIR *dir = opendir(dirname);

    if (dir == NULL)
        errx(EXIT_FAILURE, "failed to opendir: %s", dirname);

    struct dirent *entry;
    char subdir_path[1024];

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Expect subdirectories named a to z
        if (strlen(entry->d_name) != 1)
            continue;

        char c = entry->d_name[0];
        if (c < 'a' || c > 'z')
            continue;

        // Full path to the subdirectory
        snprintf(subdir_path, sizeof(subdir_path), "%s/%s", dirname,
                 entry->d_name);

        struct stat st;
        if (stat(subdir_path, &st) == -1)
            errx(EXIT_FAILURE, "failed to stat: %s", subdir_path);

        // Skip non-directories
        if (!S_ISDIR(st.st_mode))
            continue;

        // Open subdirectory
        DIR *subdir = opendir(subdir_path);
        if (subdir == NULL)
            errx(EXIT_FAILURE, "failed to opendir: %s", subdir_path);

        struct dirent *file_entry;
        char file_path[2048];

        while ((file_entry = readdir(subdir)) != NULL)
        {
            if (strcmp(file_entry->d_name, ".") == 0 ||
                strcmp(file_entry->d_name, "..") == 0)
                continue;

            snprintf(file_path, sizeof(file_path), "%s/%s", subdir_path,
                     file_entry->d_name);

            struct stat fst;
            if (stat(file_path, &fst) == -1)
                errx(EXIT_FAILURE, "failed to stat: %s", file_path);

            if (!S_ISREG(fst.st_mode))
                continue;

            // Load matrix
            Matrix *input = mat_load_from_file(file_path);
            mat_inplace_vertical_flatten(input);

            // Label = directory name ('a' → 0, etc.)
            int label = c - 'a';

            Training_Data *tuple = td_create(input, label);
            ds_add_tuple(dataset, tuple);
        }

        closedir(subdir);
    }

    closedir(dir);
    return dataset;
}

Dataset *ds_load_from_file(char *filename)
{
    FILE *file_stream = fopen(filename, "r");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file %s", filename);

    int fd = fileno(file_stream);

    int r;
    size_t size;
    size_t class;
    Matrix *m;

    r = read(fd, &size, sizeof(size_t));
    if (r != sizeof(size_t))
        errx(EXIT_FAILURE, "Failed to read dataset size");

    Dataset *ds = malloc(sizeof(Dataset));
    if (ds == NULL)
        errx(EXIT_FAILURE, "Memory allocation failed");
    ds->content = malloc(size * sizeof(Training_Data *));
    if (ds->content == NULL)
        errx(EXIT_FAILURE, "Memory allocation failed");
    ds->max_size = size;
    ds->size = size;

    for (size_t i = 0; i < size; ++i)
    {
        r = read(fd, &class, sizeof(size_t));
        if (r != sizeof(size_t))
            errx(EXIT_FAILURE, "Failed to read class");
        m = mat_load_from_fd(fd);
        mat_inplace_vertical_flatten(m);
        ds->content[i] = td_create(m, class);
    }

    return ds;
}

void ds_save_to_file(Dataset *ds, char *filename)
{
    FILE *file_stream = fopen(filename, "w");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file %s", filename);

    int fd = fileno(file_stream);

    int w;
    w = write(fd, &ds->size, sizeof(size_t));
    if (w != sizeof(size_t))
        errx(EXIT_FAILURE, "Failed to write dataset size");

    for (size_t i = 0; i < ds->size; ++i)
    {
        Training_Data *td = ds->content[i];
        w = write(fd, &td->expected_class, sizeof(size_t));
        if (w != sizeof(size_t))
            errx(EXIT_FAILURE, "Failed to write class");
        mat_save_to_fd(td->input, fd);
    }
}

Dataset *ds_load_from_compressed_file(char *filename)
{
    FILE *file_stream = fopen(filename, "rb");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file %s", filename);

    int fd = fileno(file_stream);

    int r;
    size_t size;
    char class;
    char buff;
    Matrix *m;
    float *c;

    r = read(fd, &size, sizeof(size_t));
    if (r != sizeof(size_t))
        errx(EXIT_FAILURE, "Failed to read dataset size");

    Dataset *ds = malloc(sizeof(Dataset));
    if (ds == NULL)
        errx(EXIT_FAILURE, "Memory allocation failed");
    ds->content = malloc(size * sizeof(Training_Data *));
    if (ds->content == NULL)
        errx(EXIT_FAILURE, "Memory allocation failed");
    ds->max_size = size;
    ds->size = size;

    for (size_t i = 0; i < size; ++i)
    {
        r = read(fd, &class, sizeof(char));
        if (r != sizeof(char))
            errx(EXIT_FAILURE, "Failed to read class");

        m = mat_create(784, 1);
        c = mat_coef_ptr(m, 0, 0);
        for (size_t j = 0; j < 784; j += 8)
        {
            r = read(fd, &buff, sizeof(char));
            if (r != sizeof(char))
                errx(EXIT_FAILURE, "Failed to read class");

            c[j + 0] = (buff & (1 << 7)) == 0 ? 0.0f : 1.0f;
            c[j + 1] = (buff & (1 << 6)) == 0 ? 0.0f : 1.0f;
            c[j + 2] = (buff & (1 << 5)) == 0 ? 0.0f : 1.0f;
            c[j + 3] = (buff & (1 << 4)) == 0 ? 0.0f : 1.0f;
            c[j + 4] = (buff & (1 << 3)) == 0 ? 0.0f : 1.0f;
            c[j + 5] = (buff & (1 << 2)) == 0 ? 0.0f : 1.0f;
            c[j + 6] = (buff & (1 << 1)) == 0 ? 0.0f : 1.0f;
            c[j + 7] = (buff & (1 << 0)) == 0 ? 0.0f : 1.0f;
        }

        mat_inplace_vertical_flatten(m);
        ds->content[i] = td_create(m, class);
    }

    fclose(file_stream);

    return ds;
}

void ds_save_to_compressed_file(Dataset *ds, const char *filename)
{
    FILE *file_stream = fopen(filename, "wb");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file for writing: %s", filename);

    int fd = fileno(file_stream);

    ssize_t w = write(fd, &ds->size, sizeof(size_t));
    if (w != sizeof(size_t))
        errx(EXIT_FAILURE, "Failed to write dataset size");

    for (size_t i = 0; i < ds->size; ++i)
    {
        Training_Data *td = ds->content[i];
        Matrix *m = td->input;
        float *c = mat_coef_ptr(m, 0, 0);

        char class = (char)td->expected_class;

        w = write(fd, &class, sizeof(char));
        if (w != sizeof(char))
            errx(EXIT_FAILURE, "Failed to write class byte");

        unsigned char buff;
        for (size_t j = 0; j < 28 * 28; j += 8)
        {
            buff = 0;
            for (int b = 0; b < 8; b++)
            {
                buff <<= 1;
                buff |= (c[j + b] > 0.5f);
            }
            w = write(fd, &buff, 1);
            if (w != sizeof(buff))
                errx(EXIT_FAILURE, "Failed to write compressed pixel block");
        }
    }

    fclose(file_stream);
}

inline void ds_shuffle(Dataset *dataset)
{
    shuffle_array(dataset->content, sizeof(Training_Data *), dataset->size);
}

void ds_split(Dataset *dataset, float test_percentage, Dataset **out_train,
              Dataset **out_test)
{
    *out_train = ds_create_empty();
    *out_test = ds_create_empty();

    size_t nb_samples[26] = {};
    for (size_t i = 0; i < dataset->size; ++i)
        nb_samples[dataset->content[i]->expected_class]++;

    ds_shuffle(dataset);

    size_t nb_test[26] = {};
    for (size_t i = 0; i < 26; ++i)
        nb_test[i] = (size_t)(test_percentage * nb_samples[i]);

    for (size_t i = 0; i < dataset->size; ++i)
    {
        Training_Data *td = dataset->content[i];
        size_t class = td->expected_class;
        if (nb_test[class] > 0)
        {
            ds_add_tuple(*out_test, td);
            --nb_test[class];
        }
        else
        {
            ds_add_tuple(*out_train, td);
        }
    }

    free(dataset->content);
    free(dataset);
}

inline Training_Data *ds_get_data(Dataset *dataset, size_t i)
{
    return dataset->content[i];
}

void ds_free(Dataset *dataset)
{
    for (size_t i = 0; i < dataset->size; i++)
        td_free(dataset->content[i]);
    free(dataset->content);
    free(dataset);
}
