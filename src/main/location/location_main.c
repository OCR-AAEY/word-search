#include "location/letters_extraction.h"
#include "utils/utils.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 2)
        errx(EXIT_FAILURE, "Missing arguments. For help use --help");

    if (strcmp(argv[1], "--help") == 0)
    {
        // printf("\n============= WORD SEARCH LOCATION =============\n\n"
        //        "Usage: %s [LVL] [IMG] [ANGLE]\n"
        //        "- LVL : The level of the image to load (1 or 2).\n"
        //        "- IMG : The number of the image to load (1 or 2).\n"
        //        "- ANGLE : The rotation angle in degrees to apply to the
        //        loaded " "image between -180 and 180. Positive values rotate
        //        clockwise.\n" "NOTE: If ANGLE is not a number, it is
        //        considered as 0.\n\n", argv[0]);

        printf("\n============= WORD SEARCH LOCATION =============\n\n"
               "Usage: %s [LVL] [IMG]\n"
               "- LVL : The level of the image to load (1 or 2).\n"
               "- IMG : The number of the image to load (1 or 2).\n",
               argv[0]);
        exit(EXIT_SUCCESS);
    }
    // if (argc < 4)
    //     errx(EXIT_FAILURE, "Missing arguments. For help use --help");

    if (argc < 3)
        errx(EXIT_FAILURE, "Missing arguments. For help use --help");

    char *level_arg = argv[1];
    int level = atoi(level_arg);
    if (level != 1 && level != 2)
        errx(EXIT_FAILURE, "The level argument must be either 1 or 2");

    char *image_arg = argv[2];
    int image = atoi(image_arg);
    if (image != 1 && image != 2)
        errx(EXIT_FAILURE, "The image argument must be either 1 or 2");

    char image_path[255];
    if (level == 1)
    {
        if (image == 1)
        {
            sprintf(image_path, LEVEL_1_IMG_1);
        }
        else
        {
            sprintf(image_path, LEVEL_1_IMG_2);
        }
    }
    else
    {
        if (image == 1)
        {
            sprintf(image_path, LEVEL_2_IMG_1);
        }
        else
        {
            sprintf(image_path, LEVEL_2_IMG_2);
            // sprintf(image_path, "aaaa");
        }
    }

    Point **intersection_points;
    size_t h, w;

    int status = locate_and_extract_letters_png(image_path,
                                                &intersection_points, &h, &w);
    if (status != 0)
    {
        fprintf(stderr, "Failed to locate and extract letters to png\n");
        return EXIT_FAILURE;
    }

    free_points(intersection_points, h);
    return EXIT_SUCCESS;
}
