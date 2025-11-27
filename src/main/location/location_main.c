#include "extract_char/extract_char.h"
#include "location/location.h"
#include "pretreatment/pretreatment.h"
#include "pretreatment/visualization.h"
#include "rotation/rotation.h"
#include "utils/utils.h"
#include <stdio.h>
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
        }
    }

    cleanup_folders();
    setup_folders();

    ImageData *img = load_image(image_path);
    Matrix *gray = image_to_grayscale(img);
    export_matrix(gray, GRAYSCALED_FILENAME);
    free_image(img);

    Matrix *threshold = adaptative_gaussian_thresholding(gray, 255, 7, 3, 2);
    export_matrix(threshold, THRESHOLDED_FILENAME);
    mat_free(gray);

    // TODO: threshold first
    Matrix *rotated = auto_deskew_matrix(threshold);
    export_matrix(rotated, ROTATED_FILENAME);
    mat_free(threshold);

    Matrix *closing = morph_transform(rotated, 1, Closing);
    export_matrix(closing, CLOSING_FILENAME);

    Matrix *opening = morph_transform(closing, 2, Opening);
    export_matrix(opening, OPENING_FILENAME);
    export_matrix(opening, POSTTREATMENT_FILENAME);
    mat_free(closing);

    size_t nb_lines;
    Line **lines = hough_transform_lines(rotated, 90, 5, 1, &nb_lines);
    mat_free(rotated);

    draw_lines_on_img(lines, nb_lines, POSTTREATMENT_FILENAME,
                      HOUGHLINES_VISUALIZATION_FILENAME);

    size_t width_points, height_points;
    Point **points = extract_intersection_points(lines, nb_lines,
                                                 &height_points, &width_points);

    draw_points_on_img(points, height_points, width_points,
                       HOUGHLINES_VISUALIZATION_FILENAME,
                       INTERSECTION_POINTS_FILENAME);

    extract_grid_cells(opening, points, height_points, width_points);
    BoundingBox *grid_box =
        get_bounding_box_grid(points, height_points, width_points);
    free_points(points, height_points);

    draw_boundingbox_on_img(grid_box, POSTTREATMENT_FILENAME,
                            GRID_BOUNDING_BOXES_FILENAME);
    BoundingBox *remaining_box = find_biggest_remaining_area(
        grid_box, mat_height(opening), mat_width(opening));

    size_t nb_words;
    BoundingBox **words_boxes =
        get_bounding_box_words(opening, remaining_box, 5, 20, 4, &nb_words);

    free(remaining_box);

    draw_boundingboxes_on_img(words_boxes, nb_words, POSTTREATMENT_FILENAME,
                              WORDS_BOUNDING_BOXES_FILENAME);
    extract_words(opening, words_boxes, nb_words);

    size_t *word_nb_letters;
    BoundingBox ***letters_boxes = get_bounding_box_letters(
        opening, words_boxes, nb_words, 2, &word_nb_letters);

    draw_2d_boundingboxes_on_img(letters_boxes, nb_words, word_nb_letters,
                                 POSTTREATMENT_FILENAME,
                                 LETTERS_BOUNDING_BOXES_FILENAME);
    extract_letters(opening, letters_boxes, nb_words, word_nb_letters);

    for (size_t i = 0; i < nb_words; i++)
    {
        free_bboxes(letters_boxes[i], word_nb_letters[i]);
    }
    free(letters_boxes);
    free(word_nb_letters);
    free_bboxes(words_boxes, nb_words);
    free(grid_box);

    mat_free(opening);
    free_lines(lines, nb_lines);
    return EXIT_SUCCESS;
}
