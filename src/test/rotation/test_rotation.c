#include "bounding_boxes/pretreatment.h"
#include "bounding_boxes/visualization.h"
#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "rotation/rotation.h"
#include <criterion/criterion.h>
#include <err.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DIFF 2

int compare_images_with_tolerance(ImageData *a, ImageData *b)
{
    if (a->width != b->width || a->height != b->height)
        return 0;

    for (size_t y = 0; y < a->height; y++)
    {
        for (size_t x = 0; x < a->width; x++)
        {
            Pixel *pa = &a->pixels[y * a->width + x];
            Pixel *pb = &b->pixels[y * b->width + x];

            if (abs(pa->r - pb->r) > MAX_DIFF ||
                abs(pa->g - pb->g) > MAX_DIFF || abs(pa->b - pb->b) > MAX_DIFF)
            {
                return 0;
            }
        }
    }
    return 1;
}

static void run_rotation_test(const char *file_path, double angle)
{
    ImageData *img = load_image(file_path);
    cr_assert_not_null(img, "Failed to load image: %s", file_path);

    Matrix *m = image_to_grayscale(img);
    cr_assert_not_null(m, "Failed to convert to grayscale: %s", file_path);

    Matrix *rot = rotate_matrix(m, angle);
    cr_assert_not_null(rot, "Rotation failed: %s", file_path);

    ImageData *rot_img = pixel_matrix_to_image(rot);
    cr_assert_not_null(rot_img, "Failed to convert rotated matrix to image: %s",
                       file_path);

    // Reference path: same folder with "test_" prefix
    char reference_path[1024];
    snprintf(reference_path, sizeof(reference_path),
             "assets/test_images/test_%s", strrchr(file_path, '/') + 1);
    ImageData *expected = load_image(reference_path);
    cr_assert_not_null(expected, "Reference image missing: %s", reference_path);

    // Compare with tolerance
    cr_expect(compare_images_with_tolerance(rot_img, expected),
              "Rotated image differs too much from reference: %s", file_path);

    mat_free(m);
    mat_free(rot);
    free_image(img);
    free_image(rot_img);
    free_image(expected);
}

// Individual test cases
Test(rotation_tests, level_1_image_1)
{
    run_rotation_test("assets/sample_images/level_1_image_1.png", 45.0);
}

Test(rotation_tests, level_1_image_2)
{
    run_rotation_test("assets/sample_images/level_1_image_2.png", 45.0);
}

Test(rotation_tests, level_2_image_1)
{
    run_rotation_test("assets/sample_images/level_2_image_1.png", 45.0);
}

Test(rotation_tests, level_2_image_2)
{
    run_rotation_test("assets/sample_images/level_2_image_2.png", 45.0);
}
