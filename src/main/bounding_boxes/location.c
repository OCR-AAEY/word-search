#include "bounding_boxes/location.h"
#include "bounding_boxes/hough_lines.h"
#include "bounding_boxes/pretreatment.h"
#include "bounding_boxes/visualization.h"
#include "extract_char/extract_char.h"
#include "utils/utils.h"
#include <stdio.h>

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

#ifndef UNIT_TEST

int main()
{
    cleanup_folders();
    setup_folders();

    // ImageData *img = load_image("assets/test_images/montgolfiere.jpg");
    ImageData *img = load_image(LEVEL_1_IMG_1);
    // ImageData *img = load_image(LEVEL_1_IMG_2);
    // ImageData *img = load_image(LEVEL_2_IMG_1);
    // ImageData *img = load_image(LEVEL_2_IMG_2);
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

    GError *error;
    save_pixbuf_to_png(pixbuf_result, POSTTREATMENT_FILENAME, &error);
    if (error)
        g_error_free(error);

    g_object_unref(pixbuf_result);
    free_image(result_img);

    free_image(img);

    // mat_free(blured);

    // print_lines(lines, nb_lines);

    draw_lines_on_img(lines, nb_lines, POSTTREATMENT_FILENAME,
                      HOUGHLINES_VISUALIZATION_FILENAME);

    size_t width_points, height_points;
    Point **points = extract_intersection_points(lines, nb_lines,
                                                 &height_points, &width_points);

    draw_points_on_img(points, height_points, width_points,
                       HOUGHLINES_VISUALIZATION_FILENAME,
                       INTERSECTION_POINTS_FILENAME);

    // print_points(points, nb_points);
    extract_grid_cells(threshold, points, height_points, width_points);
    mat_free(threshold);
    free_points(points, height_points);
    free_lines(lines, nb_lines);
    return EXIT_SUCCESS;
}

#endif