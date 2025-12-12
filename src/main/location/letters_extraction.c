#include "location/location.h"
#include "location/location_grid.h"
#include "location/location_word_letters.h"
#include "location/split_letters.h"

#include "pretreatment/pretreatment.h"
#include "pretreatment/visualization.h"
#include "rotation/rotation.h"
#include "utils/utils.h"

int locate_and_extract_letters_png(const char *input_image)
{
    cleanup_folders();
    setup_folders();
    int status_export;

    ImageData *img = load_image(input_image);
    if (img == NULL)
        return EXIT_FAILURE;

    Matrix *gray = image_to_grayscale(img);
    if (gray == NULL)
    {
        free_image(img);
        return EXIT_FAILURE;
    }
    status_export = export_matrix(gray, GRAYSCALED_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export grayscale\n");
    }
    free_image(img);

    Matrix *threshold = adaptative_gaussian_thresholding(gray, 255, 11, 7, 4);
    if (threshold == NULL)
    {
        mat_free(gray);
        return EXIT_FAILURE;
    }

    status_export = export_matrix(threshold, THRESHOLDED_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export thresholded\n");
    }
    mat_free(gray);

    Matrix *rotated = auto_deskew_matrix(threshold);
    if (rotated == NULL)
    {
        mat_free(threshold);
        return EXIT_FAILURE;
    }

    status_export = export_matrix(rotated, ROTATED_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export rotated\n");
    }
    mat_free(threshold);

    Matrix *closing = morph_transform(rotated, 1, Closing);
    if (closing == NULL)
    {
        mat_free(rotated);
        return EXIT_FAILURE;
    }

    status_export = export_matrix(closing, CLOSING_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export closing\n");
    }

    Matrix *opening = morph_transform(closing, 2, Opening);
    if (opening == NULL)
    {
        mat_free(closing);
        mat_free(rotated);
        return EXIT_FAILURE;
    }

    status_export = export_matrix(opening, OPENING_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export opening\n");
    }
    status_export = export_matrix(opening, POSTTREATMENT_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export post treatment\n");
    }
    mat_free(closing);

    size_t nb_lines;
    Line **lines = hough_transform_lines(rotated, 90, 5, 1, &nb_lines);
    mat_free(rotated);

    if (lines == NULL)
    {
        fprintf(stderr, "Failed to apply hough lines on the image\n");
        mat_free(opening);
        return EXIT_FAILURE;
    }

    status_export = draw_lines_on_img(lines, nb_lines, POSTTREATMENT_FILENAME,
                                      HOUGHLINES_VISUALIZATION_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export hough lines\n");
    }

    size_t width_points, height_points;
    Point **points = extract_intersection_points(lines, nb_lines,
                                                 &height_points, &width_points);
    if (points == NULL)
    {
        fprintf(stderr, "Failed to find intersection points\n");
        mat_free(opening);
        free_lines(lines, nb_lines);
        return EXIT_FAILURE;
    }

    status_export = draw_points_on_img(points, height_points, width_points,
                                       HOUGHLINES_VISUALIZATION_FILENAME,
                                       INTERSECTION_POINTS_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export intersection points\n");
    }

    int grid_cells_status =
        extract_grid_cells(opening, points, height_points, width_points);
    if (grid_cells_status != 0)
    {
        fprintf(stderr, "Failed to export grid cells\n");
        mat_free(opening);
        free_lines(lines, nb_lines);
        free_points(points, height_points);
        return EXIT_FAILURE;
    }

    BoundingBox *grid_box =
        get_bounding_box_grid(points, height_points, width_points);
    free_points(points, height_points);

    if (grid_box == NULL)
    {
        fprintf(stderr, "Failed to get grid bounding box\n");
        mat_free(opening);
        free_lines(lines, nb_lines);
        return EXIT_FAILURE;
    }

    status_export = draw_boundingbox_on_img(grid_box, POSTTREATMENT_FILENAME,
                                            GRID_BOUNDING_BOXES_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export grid bounding box\n");
    }

    BoundingBox *remaining_box = find_biggest_remaining_area(
        grid_box, mat_height(opening), mat_width(opening));

    if (remaining_box == NULL)
    {
        fprintf(stderr, "Failed to get remaining area for word list\n");
        mat_free(opening);
        free(grid_box);
        free_lines(lines, nb_lines);
        return EXIT_FAILURE;
    }

    size_t nb_words;
    BoundingBox **words_boxes =
        get_bounding_box_words(opening, remaining_box, 5, 20, 4, &nb_words);
    free(remaining_box);

    if (words_boxes == NULL)
    {
        fprintf(stderr, "Failed to get word bounding boxes\n");
        mat_free(opening);
        free(grid_box);
        free_lines(lines, nb_lines);
        return EXIT_FAILURE;
    }

    status_export =
        draw_boundingboxes_on_img(words_boxes, nb_words, POSTTREATMENT_FILENAME,
                                  WORDS_BOUNDING_BOXES_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr,
                "step export : failed to export words bounding boxes\n");
    }
    status_export = extract_words(opening, words_boxes, nb_words);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export full words as png\n");
    }

    // TODO : continue remove errx
    size_t *word_nb_letters;
    BoundingBox ***letters_boxes = get_bounding_box_letters(
        opening, words_boxes, nb_words, 2, &word_nb_letters);

    letters_boxes =
        detect_split_large_letters(letters_boxes, nb_words, word_nb_letters);

    status_export = draw_2d_boundingboxes_on_img(
        letters_boxes, nb_words, word_nb_letters, POSTTREATMENT_FILENAME,
        LETTERS_BOUNDING_BOXES_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export letters boxes\n");
    }
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