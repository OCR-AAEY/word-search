#include "wordlist_rebuild.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "ocr/neural_network.h"
#include "pretreatment/pretreatment.h"

#define MAX_PATH 2048

/* ----- STATIC HELPERS ----- */

static int detect_num_words(const char *folder)
{
    DIR *d = opendir(folder);
    if (!d)
        return -1;

    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL)
        if (entry->d_name[0] != '.')
            count++;
    closedir(d);
    return count;
}

static char ocr_letter_from_file(const char *path, Neural_Network *net)
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
    if (!tmp)
        return '?';
    m = tmp;

    mat_inplace_toggle(m);
    tmp = mat_strip_margins(m);
    mat_free(m);
    if (!tmp)
        return '?';
    m = tmp;

    tmp = mat_scale_to_28(m, 0.0f);
    mat_free(m);
    if (!tmp)
        return '?';
    m = tmp;

    mat_inplace_vertical_flatten(m);
    char c = net_decode_letter(net, m, NULL);
    mat_free(m);
    return c;
}

/* ----- PUBLIC API ----- */

Wordlist *wordlist_rebuild_from_folder(const char *folder,
                                       const char *model_path)
{
    if (!folder || !model_path)
        return NULL;

    int num_words = detect_num_words(folder);
    if (num_words <= 0)
    {
        fprintf(stderr, "No word folders found in %s\n", folder);
        return NULL;
    }

    Neural_Network *net = net_load_from_file((char *)model_path);
    if (!net)
        return NULL;

    Wordlist *wl = malloc(sizeof(Wordlist));
    if (!wl)
    {
        net_free(net);
        return NULL;
    }

    wl->count = num_words;
    wl->words = calloc((size_t)num_words, sizeof(char *));
    wl->lengths = calloc((size_t)num_words, sizeof(int));
    if (!wl->words || !wl->lengths)
    {
        free(wl->words);
        free(wl->lengths);
        free(wl);
        net_free(net);
        return NULL;
    }

    for (int i = 0; i < num_words; i++)
    {
        char letters_path[MAX_PATH];
        int n = snprintf(letters_path, sizeof letters_path,
                         "%s/word_%d/letters", folder, i);
        if (n < 0 || (size_t)n >= sizeof letters_path)
            continue;

        int letter_count = 0;
        while(1)
        {
            char letter_file[MAX_PATH];
            int m = snprintf(letter_file, sizeof letter_file, "%s/%d.png",
                             letters_path, letter_count);
            if (m < 0 || (size_t)m >= sizeof letter_file)
                break;

            FILE *f = fopen(letter_file, "r");
            if (!f)
                break;
            fclose(f);

            letter_count++;
        }

        wl->lengths[i] = letter_count;
        wl->words[i] = calloc((size_t)letter_count + 1, sizeof(char));
        if (!wl->words[i])
            continue;

        for (int j = 0; j < letter_count; j++)
        {
            char letter_file[MAX_PATH];
            int m = snprintf(letter_file, sizeof letter_file, "%s/%d.png",
                             letters_path, j);
            if (m < 0 || (size_t)m >= sizeof letter_file)
                continue;

            wl->words[i][j] = ocr_letter_from_file(letter_file, net);
        }
        wl->words[i][letter_count] = '\0';
    }

    net_free(net);
    return wl;
}

void wordlist_free(Wordlist *wl)
{
    if (!wl)
        return;
    for (int i = 0; i < wl->count; i++)
        free(wl->words[i]);
    free(wl->words);
    free(wl->lengths);
    free(wl);
}
