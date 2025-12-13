#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "neural_network.h"
#include "pretreatment/pretreatment.h"

/// @brief This can be used using:
///
/// `printf '%s\n' assets/ocr/other/cheat/cropped/*.png | shuf | while read -r
/// img; do echo "$img" && viu "$img" && echo "" && ./decode_image "$img" &&
/// read -r -p "Press Enter to continue..." </dev/tty && echo ""; done`
///
/// @param argc
/// @param argv
/// @return
int main(int argc, char **argv)
{
    if (argc != 2)
        errx(EXIT_FAILURE, "Expected 1 arg but got %i", argc - 1);

    ImageData *img = load_image(argv[1]);

    Matrix *m, *tmp;
    m = image_to_grayscale(img);
    free_image(img);

    tmp = adaptative_gaussian_thresholding(m, 1.0f, 11, 10, 5);
    mat_free(m);
    m = tmp;

    mat_inplace_to_one_hot(m);

    mat_inplace_toggle(m);

    tmp = morph_transform(m, 2, Closing);
    mat_free(m);
    m = tmp;

    tmp = morph_transform(m, 2, Opening);
    mat_free(m);
    m = tmp;

    tmp = mat_strip_margins(m);
    if (tmp == NULL)
    {
        mat_free(m);
        printf("Failed to strip margins.\n");
        return 1;
    }
    mat_free(m);
    m = tmp;

    tmp = mat_scale_to_28(m, 0.0f);
    mat_free(m);
    m = tmp;

    mat_display(m);
    printf("\n");

    mat_inplace_vertical_flatten(m);

    Neural_Network *net = net_load_from_file(
        "/home/lomination/word-search/assets/ocr/model/grid.nn");
    Matrix *res = net_feed_forward(net, m, NULL, NULL);

    for (size_t i = 0; i < 26; ++i)
    {
        printf("%c: %6.2f%% (", 'a' + (char)i, mat_coef(res, i, 0) * 100.0f);
        for (size_t j = 0; j < 100; j++)
        {
            if (j < (size_t)(mat_coef(res, i, 0) * 100.0f))
                printf("█");
            else
                printf(" ");
        }
        printf(")\n");
    }

    mat_free(m);
    mat_free(res);
    net_free(net);

    return 0;
}