#include <criterion/criterion.h>

#include "bounding_boxes/pretreatment.h"

Test(pretreatment, pixel_to_grayscale)
{
    cr_expect_eq(pixel_to_grayscale(&(Pixel){.r = 100, .g = 55, .b = 30}), 63);
    cr_expect_eq(pixel_to_grayscale(&(Pixel){.r = 255, .g = 255, .b = 255}), 255);
    cr_expect_eq(pixel_to_grayscale(&(Pixel){.r = 0, .g = 0, .b = 0}), 0);
}

Test(pretreatment, gaussian_function)
{
    cr_expect_float_eq(gaussian_function(-3, 6, 1.5), 3.211388E-6, 1E-12);
    cr_expect_float_eq(gaussian_function(2, 4, 2), 3.26606E-3, 1E-12);
    cr_expect_float_eq(gaussian_function(12, 36, 7), 1.34944E-9, 1E-12);
    cr_expect_float_eq(gaussian_function(-2, 7, 3.5), 1.49345E-3, 1E-12);
}

Test(pretreatment, gaussian_kernel)
{
    Matrix *g = create_empty_matrix(5, 5);
    g = gaussian_kernel(g, 1.5, 5);

    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            double cell = get_coef(g, i, j);
            cr_expect_float_eq(cell, gaussian_function(i, j, 1.5), 1E-12);
        }
    }
    free_matrix(g);
}

Test(pretreatment, sum_matrix_coefs)
{
    double *values = calloc(25, sizeof(double));
    for (int i = 0; i < 25; i++)
    {
        values[i] = i;
    }
    double expects = 325;
    Matrix *g = create_matrix(5, 5, values);
    double sum = sum_matrix_coefs(g);
    cr_expect_float_eq(sum, expects, 1E-12);
    free_matrix(g);
}

Test(pretreatment, gaussian_normalize)
{
    double *values = calloc(25, sizeof(double));
    for (int i = 0; i < 25; i++)
    {
        values[i] = i;
    }
    Matrix *g = create_matrix(5, 5, values);
    Matrix *c = mat_deepcopy(g);
    double sum = sum_matrix_coefs(g);
    gaussian_normalize(g);
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            double cell = get_coef(c, i, j);
            double original_cell = get_coef(g, i, j);
            cr_expect_float_eq(cell, original_cell / sum, 1E-12);
        }
    }
    free_matrix(g);
    free_matrix(c);
}

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
