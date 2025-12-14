#include "grid_rebuild.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "ocr/neural_network.h"
#include "pretreatment/pretreatment.h"

#define MAX_PATH 512

static void build_cell_path(char *out, size_t outsz, const char *folder,
                            size_t r, size_t c)
{
    snprintf(out, outsz, "%s/(%zu_%zu).png", folder, r, c);
}

static char recognize_letter_from_png(const char *path, Neural_Network *net)
{
    ImageData *img = load_image(path);
    if (!img)
        return '?';

    Matrix *m = image_to_grayscale(img);
    free_image(img);
    if (!m)
        return '?';

    Matrix *tmp = adaptative_gaussian_thresholding(m, 1.0f, 11, 10, 5);
    mat_free(m);
    m = tmp;

    mat_inplace_toggle(m);

    tmp = mat_strip_margins(m);
    mat_free(m);
    if (!tmp)
        return '?';
    m = tmp;

    tmp = mat_scale_to_28(m, 0.0f);
    mat_free(m);
    m = tmp;

    mat_inplace_vertical_flatten(m);

    char letter = net_decode_letter(net, m, NULL);

    mat_free(m);
    return letter;
}

static int detect_grid_size(const char *folder, size_t *rows, size_t *cols)
{
    DIR *d = opendir(folder);
    if (!d)
        return -1;

    struct dirent *entry;
    int max_row = -1, max_col = -1;

    while ((entry = readdir(d)) != NULL)
    {
        int r, c;
        if (sscanf(entry->d_name, "(%d_%d).png", &r, &c) == 2)
        {
            if (r > max_row)
                max_row = r;
            if (c > max_col)
                max_col = c;
        }
    }
    closedir(d);

    if (max_row < 0 || max_col < 0)
        return -1;

    *rows = max_row + 1;
    *cols = max_col + 1;
    return 0;
}

Grid *grid_rebuild_from_folder_with_model(const char *folder,
                                          const char *model_path)
{
    if (!folder || !model_path)
        return NULL;

    size_t rows = 0, cols = 0;
    if (detect_grid_size(folder, &rows, &cols) != 0)
    {
        fprintf(stderr, "Failed to detect grid size in folder: %s\n", folder);
        return NULL;
    }

    Neural_Network *net = net_load_from_file((char *)model_path);
    if (!net)
        return NULL;

    Grid *g = malloc(sizeof(Grid));
    if (!g)
    {
        net_free(net);
        return NULL;
    }

    g->height = rows;
    g->width = cols;
    g->content = calloc(rows * cols, sizeof(char));
    if (!g->content)
    {
        net_free(net);
        free(g);
        return NULL;
    }

    char path[MAX_PATH];

    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < cols; ++c)
        {
            build_cell_path(path, sizeof(path), folder, c, r);
            g->content[r * cols + c] = recognize_letter_from_png(path, net);
        }
    }

    net_free(net);
    return g;
}
