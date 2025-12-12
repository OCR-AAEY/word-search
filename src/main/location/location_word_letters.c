#include "location/location_word_letters.h"
#include "extract_char/extract_char.h"
#include "pretreatment/visualization.h"
#include "utils/utils.h"

#include <err.h>

BoundingBox *find_biggest_remaining_area(BoundingBox *grid_box,
                                         size_t src_height, size_t src_width)
{
    if ((int)src_height < grid_box->br.y || (int)src_width < grid_box->br.x)
    {
        fprintf(stderr, "find_biggest_remaining_area: The given bounding box "
                        "is outside of the bounds of the src\n");
        return NULL;
    }

    size_t top_space = grid_box->tl.y;
    size_t bottom_space = src_height - grid_box->br.y;
    size_t right_space = src_width - grid_box->br.x;
    size_t left_space = grid_box->tl.x;

    BoundingBox *remaining_area = malloc(sizeof(BoundingBox));
    if (remaining_area == NULL)
    {
        fprintf(
            stderr,
            "find_biggest_remaining_area: Failed allocation remaining area\n");
        return NULL;
    }
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
            fprintf(
                stderr,
                "find_biggest_remaining_area: There is no remaining space\n");
            return NULL;
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
            fprintf(
                stderr,
                "find_biggest_remaining_area: There is no remaining space\n");
            return NULL;
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
            fprintf(
                stderr,
                "find_biggest_remaining_area: There is no remaining space\n");
            return NULL;
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
            fprintf(
                stderr,
                "find_biggest_remaining_area: There is no remaining space\n");
            return NULL;
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

int pad_bounding_box(BoundingBox *box, size_t top, size_t bottom, size_t right,
                     size_t left)
{
    box->tl.x += left;
    if (box->tl.x > box->br.x)
    {
        fprintf(stderr, "The padding exceeds the bounding box size");
        return -1;
    }
    box->tl.y += top;
    if (box->tl.y > box->br.y)
    {
        fprintf(stderr, "The padding exceeds the bounding box size");
        return -2;
    }
    box->br.x -= right;
    if (box->br.x < box->tl.x)
    {
        fprintf(stderr, "The padding exceeds the bounding box size");
        return -3;
    }
    box->br.y -= bottom;
    if (box->br.y < box->tl.y)
    {
        fprintf(stderr, "The padding exceeds the bounding box size");
        return -4;
    }
    return EXIT_SUCCESS;
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
    {
        fprintf(
            stderr,
            "get_bounding_box_words: The size_out output parameter is NULL");
        return NULL;
    }
    if (src == NULL)
    {
        fprintf(stderr, "get_bounding_box_words: The src matrix is NULL");
        return NULL;
    }
    if (area == NULL)
    {
        fprintf(stderr, "get_bounding_box_words: The area bbox is NULL");
        return NULL;
    }

    int status;

    status = pad_bounding_box(area, area_padding, area_padding, area_padding,
                              area_padding);
    if (status != 0)
    {
        fprintf(stderr, "get_bounding_box_words: failed to pad box\n");
        return NULL;
    }

    status = draw_boundingbox_on_img(area, POSTTREATMENT_FILENAME,
                                     REMAINING_SPACE_PADDING);
    if (status != 0)
    {
        fprintf(stderr, "step export : failed to export "
                        "remaining space bounding box\n");
    }

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
        if (histogram[i] < threshold)
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
    {
        fprintf(stderr, "get_bounding_box_letters: The size_out output "
                        "parameter is NULL\n");
        return NULL;
    }
    if (src == NULL)
    {
        fprintf(stderr, "get_bounding_box_letters: The src matrix is NULL\n");
        return NULL;
    }
    if (words_boxes == NULL)
    {
        fprintf(stderr, "get_bounding_box_letters: The words_boxes is NULL\n");
        return NULL;
    }

    BoundingBox ***letters_boxes = malloc(nb_words * sizeof(BoundingBox **));
    if (!letters_boxes)
    {
        fprintf(stderr,
                "get_bounding_box_letters: Failed to allocate letters_boxes\n");
        return NULL;
    }

    *size_out = malloc(nb_words * sizeof(size_t));
    if (!*size_out)
    {
        fprintf(
            stderr,
            "get_bounding_box_letters: Failed to allocate size_out array\n");
        free(letters_boxes);
        return NULL;
    }

    for (size_t i = 0; i < nb_words; i++)
    {
        if (words_boxes[i] == NULL)
        {
            fprintf(stderr,
                    "get_bounding_box_letters: The current word box is NULL at "
                    "index %zu\n",
                    i);

            // Free previously allocated letters_boxes
            for (size_t j = 0; j < i; j++)
                free_bboxes(letters_boxes[j], (*size_out)[j]);
            free(letters_boxes);
            free(*size_out);
            *size_out = NULL;

            return NULL;
        }

        size_t histo_size;
        size_t *histogram_vert =
            histogram_vertical(src, words_boxes[i], &histo_size);
        if (!histogram_vert)
        {
            fprintf(stderr,
                    "get_bounding_box_letters: Failed to allocate "
                    "histogram_vert at index %zu\n",
                    i);

            for (size_t j = 0; j < i; j++)
                free_bboxes(letters_boxes[j], (*size_out)[j]);
            free(letters_boxes);
            free(*size_out);
            *size_out = NULL;

            return NULL;
        }

        letters_boxes[i] = find_letters_histogram_threshold(
            words_boxes[i], histogram_vert, histo_size, threshold,
            (*size_out) + i);

        free(histogram_vert);

        if (!letters_boxes[i])
        {
            fprintf(stderr,
                    "get_bounding_box_letters: Failed to find letters for word "
                    "%zu\n",
                    i);

            for (size_t j = 0; j < i; j++)
                free_bboxes(letters_boxes[j], (*size_out)[j]);
            free(letters_boxes);
            free(*size_out);
            *size_out = NULL;

            return NULL;
        }
    }

    return letters_boxes;
}

int extract_boundingbox_to_png(Matrix *src, BoundingBox *box,
                               const char *filename)
{
    return save_image_region(src, filename, box->tl.x, box->tl.y, box->br.x,
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

int extract_words(Matrix *src, BoundingBox **words_boxes, size_t nb_words)
{
    if (src == NULL)
    {
        fprintf(stderr, "extract_words: The src matrix is NULL\n");
        return -1;
    }
    if (words_boxes == NULL)
    {
        fprintf(stderr, "extract_words: The words_boxes is NULL\n");
        return -2;
    }

    setup_words_folders(nb_words);

    for (size_t word_i = 0; word_i < nb_words; word_i++)
    {
        if (words_boxes[word_i] == NULL)
        {
            fprintf(stderr, "extract_words: The current word box is NULL\n");
            return -3;
        }
        size_t filename_size =
            snprintf(NULL, 0, "%s%zu/full_word.png", WORD_BASE_DIR, word_i);
        char filename[filename_size + 1];
        sprintf(filename, "%s%zu/full_word.png", WORD_BASE_DIR, word_i);

        int status =
            extract_boundingbox_to_png(src, words_boxes[word_i], filename);
        if (status != 0)
        {
            fprintf(stderr, "extract_words: Failed to save word as png\n");
            return -4;
        }
    }
    return EXIT_SUCCESS;
}