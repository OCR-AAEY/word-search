#include "grid_rebuild.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "ocr/neural_network.h"
#include "pretreatment/pretreatment.h"

#define MAX_PATH 512

struct Grid
{
    size_t height;
    size_t width;
    char *content;
};

static void build_cell_path(char *out, size_t outsz, const char *folder,
                            size_t r, size_t c)
{
    snprintf(out, outsz, "%s/(%zu_%zu).png", folder, r, c);
}

static char recognize_letter_from_png(const char *path, Neural_Network *net)
{
    ImageData *img = load_image(path);
    if (!img)
    {
        return '?';
    }
    Matrix *m = image_to_grayscale(img);
    free_image(img);

    if (!m)
    {
        return '?';
    }

    Matrix *tmp;

    tmp = adaptative_gaussian_thresholding(m, 1.0f, 11, 10, 5);
    mat_free(m);
    m = tmp;

    mat_inplace_toggle(m); // invert image to match model compatibility

    tmp = mat_strip_margins(m);
    mat_free(m);
    if (!tmp)
        return '?';
    m = tmp;

    tmp = mat_scale_to_28(m, 0.0f);
    mat_free(m);
    m = tmp;

    mat_inplace_vertical_flatten(m);

    /* OCR */
    char letter = net_decode_letter(net, m, NULL);

    mat_free(m);
    return letter;
}

/*
 *
 * Rebuild grid from extracted grid folder using OCR model
 */
Grid *grid_rebuild_from_folder_with_model(const char *folder, size_t rows,
                                          size_t cols, const char *model_path)
{
    if (!folder || !model_path || rows == 0 || cols == 0)
        return NULL;

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

            char letter = recognize_letter_from_png(path, net);
            g->content[r * cols + c] = letter;

            printf("%c ", letter);
        }
        printf("\n");
    }

    net_free(net);
    return g;
}
