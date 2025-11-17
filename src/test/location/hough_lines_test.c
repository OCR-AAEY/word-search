#include <criterion/criterion.h>

#include "rotation/hough_lines.h"
#include "pretreatment/pretreatment.h"
#include <math.h>

Test(hough_lines, level_2_image_1)
{
    ImageData *img = load_image("assets/sample_images/level_2_image_1.png");

    Matrix *gray = image_to_grayscale(img);

    Matrix *threshold = adaptative_gaussian_thresholding(gray, 255, 11, 10, 7);
    mat_free(gray);

    Matrix *opening = morph_transform(threshold, 2, Opening);
    mat_free(threshold);

    Matrix *closing = morph_transform(opening, 2, Closing);
    mat_free(opening);

    // size_t offset =
    // (size_t)round(sqrt(mat_height(closing)*mat_height(closing) +
    // mat_width(closing)*mat_width(closing)));
    size_t nb_lines;
    Line **lines = hough_transform_lines(closing, 5, 3, 5, &nb_lines);
    // Line **lines = hough_transform_lines(closing, 1, 3, 1, &nb_lines);

    // print_lines(lines, nb_lines);
    free_lines(lines, nb_lines);
    free_image(img);

    mat_free(closing);
}

// Helper to run one test case
static void run_test_for_file(const char *file_path, size_t expected_points,
                              size_t expected_lines)
{
    cr_log_info("Testing file: %s\n", file_path);

    ImageData *img = load_image(file_path);
    cr_assert_not_null(img, "Failed to load image: %s", file_path);

    Matrix *gray = image_to_grayscale(img);
    Matrix *threshold = adaptative_gaussian_thresholding(gray, 255, 11, 10, 7);
    mat_free(gray);

    size_t nb_lines = 0;
    Line **lines = hough_transform_lines(threshold, 1, 5, 1, &nb_lines);
    cr_assert_not_null(lines, "Failed to detect lines for %s", file_path);

    size_t points_width, points_height;
    Point **points = extract_intersection_points(lines, nb_lines,
                                                 &points_height, &points_width);
    cr_assert_not_null(points, "Failed to extract intersection points for %s",
                       file_path);

    cr_expect_eq(nb_lines, expected_lines,
                 "Unexpected number of lines in %s: got %zu, expected %zu",
                 file_path, nb_lines, expected_lines);

    cr_expect_eq(points_width * points_height, expected_points,
                 "Unexpected number of points in %s: got %zu, expected %zu",
                 file_path, points_width * points_height, expected_points);

    // Cleanup
    free_lines(lines, nb_lines);
    free_points(points, points_height);
    mat_free(threshold);
    free_image(img);
}

Test(lines_intersection, level_1_image_1_png)
{
    run_test_for_file("assets/sample_images/level_1_image_1.png", 18 * 18,
                      18 + 18);
}

Test(lines_intersection, level_1_image_2_png)
{
    run_test_for_file("assets/sample_images/level_1_image_2.png", 13 * 13,
                      13 + 13);
}

Test(lines_intersection, level_2_image_1_png)
{
    run_test_for_file("assets/sample_images/level_2_image_1.png", 9 * 8, 9 + 8);
}

// Test(lines_intersection, level_2_image_2_png)
// {
//     run_test_for_file("assets/sample_images/level_2_image_2.png", 15 * 15, 15
//     + 15);
// }
