#include "bounding_boxes/location.h"
#include "bounding_boxes/hough_lines.h"
#include "bounding_boxes/pretreatment.h"
#include "bounding_boxes/visualization.h"

#ifndef UNIT_TEST

int main()
{
    // ImageData *img = load_image("assets/test_images/montgolfiere.jpg");
    // ImageData *img = load_image("assets/sample_images/level_1_image_1.png");
    // ImageData *img = load_image("assets/sample_images/level_1_image_2.png");
    // ImageData *img = load_image("assets/sample_images/level_2_image_1.png");
    ImageData *img = load_image("assets/sample_images/level_2_image_2.png");
    // ImageData *img = load_image("untitled.png");

    Matrix *gray = image_to_grayscale(img);
    // ImageData *gray_img = pixel_matrix_to_image(gray);
    // GdkPixbuf *pixbuf_gray = create_pixbuf_from_image_data(gray_img);
    // save_pixbuf_to_png(pixbuf_gray, "gray.png", NULL);
    // g_object_unref(pixbuf_gray);
    // free_image(gray_img);

    // Matrix *blured = gaussian_blur(gray, 10, 11);
    //  ImageData *blured_img = pixel_matrix_to_image(blured);
    //  GdkPixbuf *pixbuf_blur = create_pixbuf_from_image_data(blured_img);
    //  save_pixbuf_to_png(pixbuf_blur, "blured.png", NULL);
    //  g_object_unref(pixbuf_blur);
    //  free_image(blured_img);

    Matrix *threshold = adaptative_gaussian_thresholding(gray, 255, 11, 10, 5);
    mat_free(gray);

    // Matrix *opening = morph_transform(threshold, 2, Opening);
    // mat_free(threshold);

    // Matrix *closing = morph_transform(opening, 2, Closing);
    // mat_free(opening);

    // size_t offset =
    //     (size_t)round(sqrt(mat_height(closing) * mat_height(closing) +
    //                        mat_width(closing) * mat_width(closing)));
    size_t nb_lines;
    Line **lines = hough_transform_lines(threshold, 1, 5, 1, &nb_lines);

    ImageData *result_img = pixel_matrix_to_image(threshold);
    GdkPixbuf *pixbuf_result = create_pixbuf_from_image_data(result_img);
    save_pixbuf_to_png(pixbuf_result, "result.png", NULL);
    g_object_unref(pixbuf_result);
    free_image(result_img);

    free_image(img);

    // mat_free(blured);
    mat_free(threshold);

    print_lines(lines, nb_lines);

    draw_lines_on_img(lines, nb_lines, "result.png");

    size_t nb_points;
    Point **points = extract_intersection_points(lines, nb_lines, &nb_points);

    draw_points_on_img(points, nb_points, "lines_result.png");

    print_points(points, nb_points);
    free_points(points, nb_points);
    free_lines(lines, nb_lines);
    return EXIT_SUCCESS;
}

#endif