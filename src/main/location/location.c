#include "location/location.h"
#include "extract_char/extract_char.h"
#include "location/hough_lines.h"
#include "pretreatment/pretreatment.h"
#include "pretreatment/visualization.h"
#include "rotation/rotation.h"
#include "utils/utils.h"
#include <stdio.h>
#include <string.h>

void extract_grid_cells(Matrix *src, Point **points, size_t height,
                        size_t width)
{
    for (size_t h = 0; h < height - 1; h++)
    {
        for (size_t w = 0; w < width - 1; w++)
        {
            size_t filename_size =
                snprintf(NULL, 0, "%s/(%zu_%zu).png", GRID_DIR, h, w);
            char filename[filename_size + 1];
            sprintf(filename, "%s/(%zu_%zu).png", GRID_DIR, h, w);
            // printf("%s (%i, %i) to (%i, %i)\n", filename, points[h][w].x,
            // points[h][w].y,
            //                   points[h + 1][w + 1].x, points[h + 1][w +
            //                   1].y);
            save_image_region(src, filename, points[h][w].x, points[h][w].y,
                              points[h + 1][w + 1].x, points[h + 1][w + 1].y);
        }
    }
}

BoundingBox *get_bounding_box_grid(Point **points, size_t height, size_t width)
{
    if (height == 0 && width == 0)
        errx(EXIT_FAILURE,
             "Cannot find grid if there is no intersection points");
    BoundingBox *box = malloc(sizeof(BoundingBox));
    // Point top_left = (Point) {.x = points[0][0].x, .y = points[0][0].y};
    // Point bottom_right = (Point) {.x = points[height-1][width-1].x, .y =
    // points[height-1][width-1].y};
    box->tl = points[0][0];
    box->br = points[height - 1][width - 1];
    return box;
}

BoundingBox *find_biggest_remaining_area(BoundingBox *grid_box,
                                         size_t src_height, size_t src_width)
{
    if ((int)src_height < grid_box->br.y || (int)src_width < grid_box->br.x)
        errx(EXIT_FAILURE,
             "The given bounding box is outside of the bounds of the src");

    size_t top_space = grid_box->tl.y;
    size_t bottom_space = src_height - grid_box->br.y;
    size_t right_space = src_width - grid_box->br.x;
    size_t left_space = grid_box->tl.x;

    BoundingBox *remaining_area = malloc(sizeof(BoundingBox));
    Point tl = {};
    Point br = {};
    size_t maxi =
        MAX(top_space, MAX(bottom_space, MAX(left_space, right_space)));

    if (maxi == left_space)
    {
        // printf("Bigger is left\n");
        tl.x = 0;
        tl.y = 0;
        br.y = src_height - 1;
        br.x = grid_box->tl.x - 1;
        if (br.x < 0)
        {
            free(remaining_area);
            errx(EXIT_FAILURE, "There is no remaining space");
        }
    }
    else if (maxi == right_space)
    {
        // printf("Bigger is right\n");
        tl.x = grid_box->br.x + 1;
        tl.y = 0;
        if (tl.x >= (int)src_width)
        {
            free(remaining_area);
            errx(EXIT_FAILURE, "There is no remaining space");
        }
        br.x = src_width - 1;
        br.y = src_height - 1;
    }
    else if (maxi == top_space)
    {
        // printf("Bigger is top\n");
        tl.x = 0;
        tl.y = 0;
        br.x = src_width - 1;
        br.y = grid_box->tl.y - 1;
        if (tl.y < 0)
        {
            free(remaining_area);
            errx(EXIT_FAILURE, "There is no remaining space");
        }
    }
    else
    {
        // printf("Bigger is bottom\n");
        tl.x = 0;
        tl.y = grid_box->br.y + 1;
        if (tl.y >= (int)src_height)
        {
            free(remaining_area);
            errx(EXIT_FAILURE, "There is no remaining space");
        }
        br.x = src_width - 1;
        br.y = src_height - 1;
    }

    remaining_area->tl = tl;
    remaining_area->br = br;
    // printf("Remaining area : (%i, %i) to (%i, %i)\n", remaining_area->tl.x,
    // remaining_area->tl.y, remaining_area->br.x, remaining_area->br.y);
    return remaining_area;
}

size_t *histogram_horizontal(Matrix *src, BoundingBox *area, size_t *size_out)
{
    if (area == NULL)
        errx(EXIT_FAILURE, "Sigma must be positive");
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");
    if (size_out == NULL)
        errx(EXIT_FAILURE, "The size_out output parameter is NULL");

    size_t height = mat_height(src);
    size_t width = mat_width(src);
    if (area->br.y >= (int)height || area->br.y >= (int)width)
        errx(EXIT_FAILURE,
             "The area concerned is outside of the bounds of the src matrix");

    *size_out = area->br.y - area->tl.y + 1;
    size_t vert_size = area->br.x - area->tl.x + 1;
    size_t *histogram = calloc(*size_out, sizeof(size_t));

    for (size_t h = 0; h < *size_out; h++)
    {
        for (size_t w = 0; w < vert_size; w++)
        {
            float pixel = mat_coef(src, h + area->tl.y, w + area->tl.x);
            if (pixel == 0.0f)
            {
                histogram[h] += 1;
            }
        }
    }
    return histogram;
}

void pad_bounding_box(BoundingBox *box, size_t top, size_t bottom, size_t right,
                      size_t left)
{
    box->tl.x += left;
    if (box->tl.x > box->br.x)
        errx(EXIT_FAILURE, "The padding exceeds the bounding box size");
    box->tl.y += top;
    if (box->tl.y > box->br.y)
        errx(EXIT_FAILURE, "The padding exceeds the bounding box size");
    box->br.x -= right;
    if (box->br.x < box->tl.x)
        errx(EXIT_FAILURE, "The padding exceeds the bounding box size");
    box->br.y -= bottom;
    if (box->br.y < box->tl.y)
        errx(EXIT_FAILURE, "The padding exceeds the bounding box size");
}

void margin_bounding_box(BoundingBox *box, size_t top, size_t bottom,
                         size_t right, size_t left)
{
    box->tl.x -= left;
    if (box->tl.x < 0)
        box->tl.x = 0;
    box->tl.y -= top;
    if (box->tl.y < 0)
        box->tl.y = 0;
    box->br.x += right;
    box->br.y += bottom;
}

BoundingBox **find_words_histogram_threshold(BoundingBox *area,
                                             size_t *histogram, size_t size,
                                             size_t threshold, size_t *size_out)
{
    if (histogram == NULL)
        errx(EXIT_FAILURE, "The histogram is NULL");
    if (size_out == NULL)
        errx(EXIT_FAILURE, "The size_out output parameter is NULL");
    size_t max_boxes = 10;
    size_t boxes_index = 0;
    BoundingBox **words_boxes = malloc(max_boxes * sizeof(BoundingBox *));
    BoundingBox *current_box = NULL;

    for (size_t i = 0; i < size; i++)
    {
        if (histogram[i] <= threshold)
        {
            if (current_box != NULL)
            {
                current_box->br.y = area->tl.y + i - 1;
                current_box = NULL;
            }
        }
        else if (current_box == NULL)
        {
            if (boxes_index == max_boxes)
            {
                max_boxes += 10;
                BoundingBox **tmp =
                    realloc(words_boxes, max_boxes * sizeof(BoundingBox *));
                if (tmp == NULL)
                {
                    free(words_boxes);
                    errx(EXIT_FAILURE,
                         "Failed to reallocate the words bounding boxes array");
                }
                words_boxes = tmp;
            }
            current_box = malloc(sizeof(BoundingBox));
            current_box->tl.y = area->tl.y + i;
            current_box->tl.x = area->tl.x;
            current_box->br.x = area->br.x;
            words_boxes[boxes_index++] = current_box;
        }
    }
    *size_out = boxes_index;
    return words_boxes;
}

BoundingBox **get_bounding_box_words(Matrix *src, BoundingBox *area,
                                     size_t threshold, size_t area_padding,
                                     size_t word_margin, size_t *size_out)
{
    if (size_out == NULL)
        errx(EXIT_FAILURE, "The size_out output parameter is NULL");
    if (src == NULL)
        errx(EXIT_FAILURE, "The src matrix is NULL");
    if (area == NULL)
        errx(EXIT_FAILURE, "The area bbox is NULL");

    pad_bounding_box(area, area_padding, area_padding, area_padding,
                     area_padding);
    draw_boundingbox_on_img(area, POSTTREATMENT_FILENAME,
                            REMAINING_SPACE_PADDING);

    size_t histo_size;
    size_t *histogram_horiz = histogram_horizontal(src, area, &histo_size);
    BoundingBox **words_boxes = find_words_histogram_threshold(
        area, histogram_horiz, histo_size, threshold, size_out);
    for (size_t i = 0; i < *size_out; i++)
    {
        margin_bounding_box(words_boxes[i], word_margin, word_margin, 0, 0);
    }
    free(histogram_horiz);
    return words_boxes;
}

size_t *histogram_vertical(Matrix *src, BoundingBox *area, size_t *size_out)
{
    if (area == NULL)
        errx(EXIT_FAILURE, "Sigma must be positive");
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");
    if (size_out == NULL)
        errx(EXIT_FAILURE, "The size_out output parameter is NULL");

    size_t height = mat_height(src);
    size_t width = mat_width(src);
    if (area->br.y >= (int)height || area->br.y >= (int)width)
        errx(EXIT_FAILURE,
             "The area concerned is outside of the bounds of the src matrix");

    size_t horiz_size = area->br.y - area->tl.y + 1;
    *size_out = area->br.x - area->tl.x + 1;
    size_t *histogram = calloc(*size_out, sizeof(size_t));

    for (size_t h = 0; h < horiz_size; h++)
    {
        for (size_t w = 0; w < *size_out; w++)
        {
            float pixel = mat_coef(src, h + area->tl.y, w + area->tl.x);
            if (pixel == 0.0f)
            {
                histogram[w] += 1;
            }
        }
    }
    return histogram;
}

BoundingBox **find_letters_histogram_threshold(BoundingBox *area,
                                               size_t *histogram, size_t size,
                                               size_t threshold,
                                               size_t *size_out)
{
    if (histogram == NULL)
        errx(EXIT_FAILURE, "The histogram is NULL");
    if (size_out == NULL)
        errx(EXIT_FAILURE, "The size_out output parameter is NULL");
    size_t max_boxes = 10;
    size_t boxes_index = 0;
    BoundingBox **letters_boxes = malloc(max_boxes * sizeof(BoundingBox *));
    BoundingBox *current_box = NULL;

    for (size_t i = 0; i < size; i++)
    {
        if (histogram[i] <= threshold)
        {
            if (current_box != NULL)
            {
                current_box->br.x = area->tl.x + i - 1;
                current_box = NULL;
            }
        }
        else if (current_box == NULL)
        {
            if (boxes_index == max_boxes)
            {
                max_boxes += 10;
                BoundingBox **tmp =
                    realloc(letters_boxes, max_boxes * sizeof(BoundingBox *));
                if (tmp == NULL)
                {
                    free(letters_boxes);
                    errx(EXIT_FAILURE,
                         "Failed to reallocate the words bounding boxes array");
                }
                letters_boxes = tmp;
            }
            current_box = malloc(sizeof(BoundingBox));
            current_box->tl.y = area->tl.y;
            current_box->tl.x = area->tl.x + i;
            current_box->br.y = area->br.y;
            letters_boxes[boxes_index++] = current_box;
        }
    }
    *size_out = boxes_index;
    return letters_boxes;
}

BoundingBox ***get_bounding_box_letters(Matrix *src, BoundingBox **words_boxes,
                                        size_t nb_words, size_t threshold,
                                        size_t **size_out)
{
    if (size_out == NULL)
        errx(EXIT_FAILURE, "The size_out output parameter is NULL");
    if (src == NULL)
        errx(EXIT_FAILURE, "The src matrix is NULL");
    if (words_boxes == NULL)
        errx(EXIT_FAILURE, "The words_boxes is NULL");

    size_t histo_size;
    BoundingBox ***letters_boxes = malloc(nb_words * sizeof(BoundingBox **));
    *size_out = malloc(nb_words * sizeof(size_t));

    for (size_t i = 0; i < nb_words; i++)
    {
        if (words_boxes[i] == NULL)
            errx(EXIT_FAILURE, "The current word box is NULL");

        size_t *histogram_horiz =
            histogram_vertical(src, words_boxes[i], &histo_size);
        letters_boxes[i] = find_letters_histogram_threshold(
            words_boxes[i], histogram_horiz, histo_size, threshold,
            (*size_out) + i);
        free(histogram_horiz);
    }

    return letters_boxes;
}

void extract_boundingbox_to_png(Matrix *src, BoundingBox *box,
                                const char *filename)
{
    save_image_region(src, filename, box->tl.x, box->tl.y, box->br.x,
                      box->br.y);
}

void extract_letters(Matrix *src, BoundingBox ***letter_boxes, size_t nb_words,
                     size_t *words_nb_letters)
{
    if (words_nb_letters == NULL)
        errx(EXIT_FAILURE, "The words_nb_letters is NULL");
    if (src == NULL)
        errx(EXIT_FAILURE, "The src matrix is NULL");
    if (letter_boxes == NULL)
        errx(EXIT_FAILURE, "The letter_boxes is NULL");

    for (size_t word_i = 0; word_i < nb_words; word_i++)
    {
        if (letter_boxes[word_i] == NULL)
            errx(EXIT_FAILURE,
                 "The current word array of letter boxes is NULL");
        for (size_t letter_index = 0; letter_index < words_nb_letters[word_i];
             letter_index++)
        {
            size_t filename_size =
                snprintf(NULL, 0, "%s%zu/%s/%zu.png", WORD_BASE_DIR, word_i,
                         LETTERS_DIR, letter_index);
            char filename[filename_size + 1];
            sprintf(filename, "%s%zu/%s/%zu.png", WORD_BASE_DIR, word_i,
                    LETTERS_DIR, letter_index);

            extract_boundingbox_to_png(src, letter_boxes[word_i][letter_index],
                                       filename);
        }
    }
}

void extract_words(Matrix *src, BoundingBox **words_boxes, size_t nb_words)
{
    if (src == NULL)
        errx(EXIT_FAILURE, "The src matrix is NULL");
    if (words_boxes == NULL)
        errx(EXIT_FAILURE, "The words_boxes is NULL");

    setup_words_folders(nb_words);

    for (size_t word_i = 0; word_i < nb_words; word_i++)
    {
        if (words_boxes[word_i] == NULL)
            errx(EXIT_FAILURE, "The current word box is NULL");
        size_t filename_size =
            snprintf(NULL, 0, "%s%zu/full_word.png", WORD_BASE_DIR, word_i);
        char filename[filename_size + 1];
        sprintf(filename, "%s%zu/full_word.png", WORD_BASE_DIR, word_i);

        extract_boundingbox_to_png(src, words_boxes[word_i], filename);
    }
}

void free_bboxes(BoundingBox **boxes, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        free(boxes[i]);
    }
    free(boxes);
}

void cleanup_folders()
{
    char cmd[255];
    sprintf(cmd, "rm -rf %s", EXTRACT_DIR);
    execute_command(cmd);
}

void setup_folders()
{
    char cmd[255];
    int status;
    sprintf(cmd, "mkdir %s", EXTRACT_DIR);
    status = execute_command(cmd);
    if (status == EXIT_FAILURE)
        errx(EXIT_FAILURE, "Command create extract dir failed");
    sprintf(cmd, "mkdir %s", WORDS_DIR);
    status = execute_command(cmd);
    if (status == EXIT_FAILURE)
        errx(EXIT_FAILURE, "Command create words dir failed");
    sprintf(cmd, "mkdir %s", GRID_DIR);
    status = execute_command(cmd);
    if (status == EXIT_FAILURE)
        errx(EXIT_FAILURE, "Command create grid dir failed");
    sprintf(cmd, "mkdir %s", EXAMPLES_DIR);
    status = execute_command(cmd);
    if (status == EXIT_FAILURE)
        errx(EXIT_FAILURE, "Command create examples failed failed");
}

void setup_words_folders(size_t nb_words)
{
    char cmd[255];
    int status;
    for (size_t i = 0; i < nb_words; i++)
    {
        sprintf(cmd, "mkdir %s%zu", WORD_BASE_DIR, i);
        status = execute_command(cmd);
        if (status == EXIT_FAILURE)
            errx(EXIT_FAILURE, "Command create %s%zu dir failed", WORD_BASE_DIR,
                 i);

        sprintf(cmd, "mkdir %s%zu/%s", WORD_BASE_DIR, i, LETTERS_DIR);
        status = execute_command(cmd);
        if (status == EXIT_FAILURE)
            errx(EXIT_FAILURE, "Command create %s%zu/%s dir failed",
                 WORD_BASE_DIR, i, LETTERS_DIR);
    }
}

/*
#ifndef UNIT_TEST

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

    float angle;
    // char *angle_arg = argv[3];
    // float angle = atof(angle_arg);
    // if (ABS(angle) > 180.0f)
    //     errx(EXIT_FAILURE,
    //          "The angle argument must be a float between -180 and 180");

    char image_path[255];
    if (level == 1)
    {
        if (image == 1)
        {
            sprintf(image_path, LEVEL_1_IMG_1);
            angle = 0;
        }
        else
        {
            sprintf(image_path, LEVEL_1_IMG_2);
            angle = 0;
        }
    }
    else
    {
        if (image == 1)
        {
            sprintf(image_path, LEVEL_2_IMG_1);
            angle = 25;
        }
        else
        {
            sprintf(image_path, LEVEL_2_IMG_2);
            angle = -4;
        }
    }

    cleanup_folders();
    setup_folders();

    ImageData *img = load_image(image_path);
    Matrix *gray = image_to_grayscale(img);
    export_matrix(gray, GRAYSCALED_FILENAME);
    free_image(img);

    Matrix *rotated = angle != 0 ? rotate_matrix(gray, angle) : gray;
    export_matrix(rotated, ROTATED_FILENAME);
    if (angle != 0)
        mat_free(gray);

    Matrix *threshold =
        adaptative_gaussian_thresholding(rotated, 255, 11, 10, 5);
    export_matrix(threshold, THRESHOLDED_FILENAME);
    mat_free(rotated);

    Matrix *closing = morph_transform(threshold, 2, Closing);
    export_matrix(closing, CLOSING_FILENAME);

    Matrix *opening = morph_transform(closing, 2, Opening);
    export_matrix(opening, OPENING_FILENAME);
    export_matrix(opening, POSTTREATMENT_FILENAME);

    mat_free(closing);

    size_t nb_lines;
    Line **lines = hough_transform_lines(threshold, 90, 5, 1, &nb_lines);
    mat_free(threshold);

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
        opening, words_boxes, nb_words, 0, &word_nb_letters);
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

#endif
*/
