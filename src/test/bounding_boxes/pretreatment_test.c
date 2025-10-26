#include <criterion/criterion.h>

#include "bounding_boxes/pretreatment.h"
#include <math.h>

Test(pretreatment, pixel_to_grayscale)
{
    cr_expect_eq(pixel_to_grayscale(&(Pixel){.r = 100, .g = 55, .b = 30}), 63);
    cr_expect_eq(pixel_to_grayscale(&(Pixel){.r = 255, .g = 255, .b = 255}),
                 255);
    cr_expect_eq(pixel_to_grayscale(&(Pixel){.r = 0, .g = 0, .b = 0}), 0);
}

// Test(pretreatment, gaussian_function)
// {
//     // cr_expect_float_eq(gaussian_function(-3, 6, 1.5), 3.211388E-6);
//     // cr_expect_float_eq(gaussian_function(2, 4, 2), 3.26606E-3, 1E-12);
//     // cr_expect_float_eq(gaussian_function(12, 36, 7), 1.34944E-9, 1E-12);
//     // cr_expect_float_eq(gaussian_function(-2, 7, 3.5), 1.49345E-3, 1E-12);
// }

Test(pretreatment, clamp)
{
    cr_expect_eq(clamp(-20, 0, 10), 0);
    cr_expect_eq(clamp(5, 0, 10), 5);
    cr_expect_eq(clamp(10, 0, 10), 10);
    cr_expect_eq(clamp(20, 0, 10), 10);
    cr_expect_eq(clamp(0, 0, 10), 0);
    cr_expect_eq(clamp(256, 42, 42), 42);
}

Test(pretreatment, clamp_failure, .exit_code = EXIT_FAILURE)
{
    clamp(-20, 10, 0);
}

Test(pretreatment, hough_lines)
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

    print_lines(lines, nb_lines);

    free_image(img);

    mat_free(closing);
}
