#include "location/location.h"
#include "location/location_grid.h"
#include "location/location_word_letters.h"
#include "location/split_letters.h"

#include "pretreatment/pretreatment.h"
#include "pretreatment/visualization.h"
#include "rotation/rotation.h"
#include "utils/utils.h"

/// @brief Loads an input image and performs full preprocessing:
/// grayscale conversion, adaptive thresholding, deskewing,
/// and morphological postprocessing.
/// @param[in] input_image Path to the input image.
/// @param[out] rotated_out Will contain the deskewed matrix (non-NULL on
/// success).
/// @return Pointer to the preprocessed Matrix on success,
///         or NULL on failure.
/// @note The caller is responsible for freeing the returned Matrix.
Matrix *load_and_preprocess_image(const char *input_image, Matrix **rotated_out)
{
    int status_export;
    *rotated_out = NULL;

    ImageData *img = load_image(input_image);
    if (img == NULL)
        return NULL;

    Matrix *gray = image_to_grayscale(img);
    if (gray == NULL)
    {
        free_image(img);
        return NULL;
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
        return NULL;
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
        return NULL;
    }

    *rotated_out = rotated;

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
        return NULL;
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
        return NULL;
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
    return opening;
}

/// @brief Runs Hough line detection on the rotated image and extracts
/// intersections.
/// @param rotated Matrix returned by auto_deskew_matrix().
/// @param width_out Output: cols of intersection grid.
/// @param height_out Output: rows of intersection grid.
/// @return 2D array of intersection point lists or NULL.
static Point **detect_grid_points(Matrix *rotated, size_t *width_out,
                                  size_t *height_out)
{
    size_t nb_lines = 0;
    Line **lines = hough_transform_lines(rotated, 90, 5, 1, &nb_lines);
    if (!lines)
        return NULL;

    draw_lines_on_img(lines, nb_lines, ROTATED_FILENAME,
                      HOUGHLINES_VISUALIZATION_FILENAME);

    Point **points =
        extract_intersection_points(lines, nb_lines, height_out, width_out);

    if (!points)
    {
        free_lines(lines, nb_lines);
        return NULL;
    }

    draw_points_on_img(points, *height_out, *width_out,
                       HOUGHLINES_VISUALIZATION_FILENAME,
                       INTERSECTION_POINTS_FILENAME);

    free_lines(lines, nb_lines);
    return points;
}

/// @brief Extracts words from the remaining area, detects letters inside each
/// word,
///        draws bounding boxes, and exports all letters as PNG.
/// @param processed_img Preprocessed image matrix.
/// @param remaining_box Bounding box of the area containing words.
/// @return 0 on success, non-zero on failure.
static int extract_words_and_letters(Matrix *processed_img,
                                     BoundingBox *remaining_box)
{
    if (processed_img == NULL || remaining_box == NULL)
    {
        fprintf(stderr, "extract_words_and_letters: invalid arguments\n");
        return -1;
    }

    int status_export;

    /// ======== Extract word bounding boxes ========
    size_t nb_words;
    BoundingBox **words_boxes = get_bounding_box_words(
        processed_img, remaining_box, 5, 20, 4, &nb_words);

    if (words_boxes == NULL)
    {
        fprintf(stderr, "Failed to get word bounding boxes\n");
        mat_free(processed_img);
        return -2;
    }

    status_export =
        draw_boundingboxes_on_img(words_boxes, nb_words, POSTTREATMENT_FILENAME,
                                  WORDS_BOUNDING_BOXES_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr,
                "step export : failed to export words bounding boxes\n");
    }
    status_export = extract_words(processed_img, words_boxes, nb_words);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export full words as png\n");
    }

    /// ======== Extract letters bounding boxes ========

    size_t *word_nb_letters;
    BoundingBox ***letters_boxes = get_bounding_box_letters(
        processed_img, words_boxes, nb_words, 2, &word_nb_letters);

    if (letters_boxes == NULL)
    {
        fprintf(stderr, "Failed to get letters bounding boxes\n");
        mat_free(processed_img);
        free_bboxes(words_boxes, nb_words);
        return -3;
    }

    /// ======== Split merged letters ========

    letters_boxes =
        detect_split_large_letters(letters_boxes, nb_words, word_nb_letters);

    if (letters_boxes == NULL)
    {
        fprintf(stderr, "Failed to split merged letters bounding boxes\n");
        mat_free(processed_img);

        for (size_t i = 0; i < nb_words; i++)
        {
            free_bboxes(letters_boxes[i], word_nb_letters[i]);
        }
        free(letters_boxes);
        free(word_nb_letters);
        free_bboxes(words_boxes, nb_words);
        return -4;
    }

    /// ======== Draw letters bounding boxes ========

    status_export = draw_2d_boundingboxes_on_img(
        letters_boxes, nb_words, word_nb_letters, POSTTREATMENT_FILENAME,
        LETTERS_BOUNDING_BOXES_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export letters boxes\n");
    }

    /// ======== Save detected letters as PNG ========

    int status_extract_letters = extract_letters(processed_img, letters_boxes,
                                                 nb_words, word_nb_letters);

    /// ======== Cleanup ========

    for (size_t i = 0; i < nb_words; i++)
    {
        free_bboxes(letters_boxes[i], word_nb_letters[i]);
    }
    free(letters_boxes);
    free(word_nb_letters);
    free_bboxes(words_boxes, nb_words);

    if (status_extract_letters)
    {
        fprintf(stderr, "Failed to export words letters as png\n");
        return -5;
    }

    return EXIT_SUCCESS;
}

int locate_and_extract_letters_png(const char *input_image)
{
    cleanup_folders();
    setup_folders();
    int status_export;

    Matrix *rotated = NULL;
    Matrix *processed_img = load_and_preprocess_image(input_image, &rotated);

    if (processed_img == NULL || rotated == NULL)
    {
        fprintf(stderr, "Failed to preprocess the image\n");
        return EXIT_FAILURE;
    }

    size_t h_points = 0, w_points = 0;
    Point **points = detect_grid_points(rotated, &w_points, &h_points);
    mat_free(rotated);
    if (points == NULL)
    {
        mat_free(processed_img);
        return EXIT_FAILURE;
    }

    BoundingBox *grid_box = get_bounding_box_grid(points, h_points, w_points);
    free_points(points, h_points);

    if (grid_box == NULL)
    {
        fprintf(stderr, "Failed to get grid bounding box\n");
        mat_free(processed_img);
        return EXIT_FAILURE;
    }

    status_export = draw_boundingbox_on_img(grid_box, POSTTREATMENT_FILENAME,
                                            GRID_BOUNDING_BOXES_FILENAME);
    if (status_export != 0)
    {
        fprintf(stderr, "step export : failed to export grid bounding box\n");
    }

    BoundingBox *remaining_box = find_biggest_remaining_area(
        grid_box, mat_height(processed_img), mat_width(processed_img));
    free(grid_box);
    if (remaining_box == NULL)
    {
        fprintf(stderr, "Failed to get remaining area for word list\n");
        mat_free(processed_img);
        return EXIT_FAILURE;
    }

    int status = extract_words_and_letters(processed_img, remaining_box);

    free(remaining_box);
    mat_free(processed_img);

    return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}