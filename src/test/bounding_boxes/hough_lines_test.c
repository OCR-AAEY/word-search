#include <criterion/criterion.h>

#include "bounding_boxes/hough_lines.h"
#include "bounding_boxes/pretreatment.h"
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

    print_lines(lines, nb_lines);

    free_image(img);

    mat_free(closing);
}
