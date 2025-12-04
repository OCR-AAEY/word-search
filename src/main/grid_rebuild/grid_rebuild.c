#include "grid_rebuild.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "neural_network/neural_network.h"
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
    if (!folder)
    {
        out[0] = '\0';
        return;
    }
    size_t n = strlen(folder);
    if (n > 0 && folder[n - 1] == '/')
        snprintf(out, outsz, "%s(%zu_%zu).png", folder, r, c);
    else
        snprintf(out, outsz, "%s/(%zu_%zu).png", folder, r, c);
}

static Matrix *mat_resize_nearest_square(const Matrix *src, size_t s)
{
    if (!src)
        return NULL;
    return mat_create_uniform_random(1, 1, 0, 0);
}

static Matrix *flatten_to_column(const Matrix *m)
{
    if (!m)
        return NULL;
    size_t h = mat_height(m);
    size_t w = mat_width(m);
    Matrix *col = mat_create_zero(h * w, 1);
    if (!col)
        return NULL;
    for (size_t r = 0; r < h; ++r)
    {
        for (size_t c = 0; c < w; ++c)
        {
            *mat_coef_ptr(col, r * w + c, 0) = mat_coef(m, r, c);
        }
    }
    return col;
}

static void normalize_column_if_needed(Matrix *col)
{
    if (!col)
        return;
    float maxv = mat_coef(col, 0, 0);
    for (size_t i = 1; i < mat_height(col); ++i)
    {
        float v = mat_coef(col, i, 0);
        if (v > maxv)
            maxv = v;
    }
    if (maxv > 1.5f)
    {
        mat_inplace_scalar_multiplication(col, 1.0f / 255.0f);
    }
}

char recognize_letter_from_png(const char *path, Neural_Network *net)
{
    if (!path || !net)
        return '?';

    ImageData *img = load_image(path);
    if (!img)
    {
        return '?';
    }

    Matrix *gray = image_to_grayscale(img);
    free_image(img);
    if (!gray)
        return '?';

    size_t net_in = net_layer_height(net, 0);

    Matrix *input_col = NULL;

    size_t s = 0;
    for (size_t cand = 1; cand * cand <= net_in; ++cand)
    {
        if (cand * cand == net_in)
        {
            s = cand;
            break;
        }
    }

    if (s > 0)
    {
        Matrix *resized = NULL;
        if (mat_height(gray) == s && mat_width(gray) == s)
        {
            resized = mat_deepcopy(gray);
        }
        else
        {
            resized = mat_create_zero(s, s);
            size_t h = mat_height(gray), w = mat_width(gray);
            for (size_t r = 0; r < s; ++r)
            {
                size_t sr = (size_t)((double)r * (double)h / (double)s);
                if (sr >= h)
                    sr = h - 1;
                for (size_t c = 0; c < s; ++c)
                {
                    size_t sc = (size_t)((double)c * (double)w / (double)s);
                    if (sc >= w)
                        sc = w - 1;
                    *mat_coef_ptr(resized, r, c) = mat_coef(gray, sr, sc);
                }
            }
        }
        mat_free(gray);
        input_col = flatten_to_column(resized);
        mat_free(resized);
    }
    else
    {
        Matrix *flat = flatten_to_column(gray);
        mat_free(gray);
        if (!flat)
            return '?';
        if (mat_height(flat) == net_in && mat_width(flat) == 1)
        {
            input_col = flat;
        }
        else
        {
            Matrix *tmp = mat_create_zero(net_in, 1);
            size_t copy = mat_height(flat) < net_in ? mat_height(flat) : net_in;
            for (size_t i = 0; i < copy; ++i)
                *mat_coef_ptr(tmp, i, 0) = mat_coef(flat, i, 0);
            mat_free(flat);
            input_col = tmp;
        }
    }

    if (!input_col)
        return '?';

    normalize_column_if_needed(input_col);

    Matrix *out = net_feed_forward(net, input_col, NULL, NULL);
    mat_free(input_col);

    if (!out)
        return '?';

    size_t best = 0;
    float bestv = mat_coef(out, 0, 0);
    for (size_t i = 1; i < mat_height(out); ++i)
    {
        float v = mat_coef(out, i, 0);
        if (v > bestv)
        {
            bestv = v;
            best = i;
        }
    }
    mat_free(out);

    if (best < 26)
        return (char)('A' + (int)best);
    return '?';
}

/*
returns a grid with recognized letters
 */
Grid *grid_rebuild_from_folder_with_model(const char *folder, size_t rows,
                                          size_t cols, const char *model_path)
{
    if (!folder || !model_path || rows == 0 || cols == 0)
        return NULL;

    Neural_Network *net = net_load_from_file((char *)model_path);
    if (!net)
    {
        fprintf(stderr, "Failed to load neural network model: %s\n",
                model_path);
        return NULL;
    }

    Grid *g = malloc(sizeof(Grid));
    if (!g)
    {
        net_free(net);
        return NULL;
    }
    g->height = rows;
    g->width = cols;
    size_t total = rows * cols;
    g->content = calloc(total, sizeof(char));
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
            build_cell_path(path, sizeof(path), folder, r, c);
            char letter = recognize_letter_from_png(path, net);
            g->content[r * cols + c] = letter;
        }
    }

    net_free(net);
    return g;
}
