#include "wordlist_rebuild.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "ocr/neural_network.h"
#include "pretreatment/pretreatment.h"

#define MAX_PATH 2048

static int count_words(const char *folder)
{
    DIR *dir = opendir(folder);
    if (dir == NULL)
    {
        return -1;
    }

    int total = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] != '.')
        {
            total++;
        }
    }

    closedir(dir);
    return total;
}

static char read_letter(const char *path, Neural_Network *net)
{
    ImageData *img = load_image(path);
    if (img == NULL)
    {
        return '?';
    }

    Matrix *mat = image_to_grayscale(img);
    free_image(img);
    if (mat == NULL)
    {
        return '?';
    }

    Matrix *tmp = adaptative_gaussian_thresholding(mat, 1.0f, 11, 10, 5);
    mat_free(mat);
    if (tmp == NULL)
    {
        return '?';
    }
    mat = tmp;

    mat_inplace_toggle(mat);
    tmp = mat_strip_margins(mat);
    mat_free(mat);
    if (tmp == NULL)
    {
        return '?';
    }
    mat = tmp;

    tmp = mat_scale_to_28(mat, 0.0f);
    mat_free(mat);
    if (tmp == NULL)
    {
        return '?';
    }
    mat = tmp;

    mat_inplace_vertical_flatten(mat);
    char c = net_decode_letter(net, mat, NULL);
    mat_free(mat);

    return c;
}

Wordlist *wordlist_rebuild_from_folder(const char *folder,
                                       const char *model_path)
{
    if (folder == NULL || model_path == NULL)
    {
        return NULL;
    }

    int total_words = count_words(folder);
    if (total_words <= 0)
    {
        fprintf(stderr, "No words in %s\n", folder);
        return NULL;
    }

    Neural_Network *net = net_load_from_file((char *)model_path);
    if (net == NULL)
    {
        return NULL;
    }

    Wordlist *wl = malloc(sizeof(Wordlist));
    if (wl == NULL)
    {
        net_free(net);
        return NULL;
    }

    wl->count = total_words;
    wl->words = calloc((size_t)total_words, sizeof(char *));
    wl->lengths = calloc((size_t)total_words, sizeof(int));
    if (wl->words == NULL || wl->lengths == NULL)
    {
        free(wl->words);
        free(wl->lengths);
        free(wl);
        net_free(net);
        return NULL;
    }

    for (int w = 0; w < total_words; w++)
    {
        char path[MAX_PATH];
        int n = snprintf(path, sizeof(path), "%s/word_%d/letters", folder, w);
        if (n < 0 || (size_t)n >= sizeof(path))
        {
            continue;
        }

        int letters = 0;
        while (1)
        {
            char file[MAX_PATH];
            int m = snprintf(file, sizeof(file), "%s/%d.png", path, letters);
            if (m < 0 || (size_t)m >= sizeof(file))
            {
                break;
            }

            FILE *f = fopen(file, "r");
            if (f == NULL)
            {
                break;
            }
            fclose(f);

            letters++;
        }

        wl->lengths[w] = letters;
        wl->words[w] = calloc((size_t)letters + 1, sizeof(char));
        if (wl->words[w] == NULL)
        {
            continue;
        }

        for (int l = 0; l < letters; l++)
        {
            char file[MAX_PATH];
            int m = snprintf(file, sizeof(file), "%s/%d.png", path, l);
            if (m < 0 || (size_t)m >= sizeof(file))
            {
                continue;
            }

            wl->words[w][l] = read_letter(file, net);
        }
        wl->words[w][letters] = '\0';
    }

    net_free(net);
    return wl;
}

void wordlist_free(Wordlist *wl)
{
    if (wl == NULL)
    {
        return;
    }

    for (int i = 0; i < wl->count; i++)
    {
        free(wl->words[i]);
    }

    free(wl->words);
    free(wl->lengths);
    free(wl);
}
