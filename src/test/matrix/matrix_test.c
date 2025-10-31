#include <criterion/criterion.h>
#include <stdio.h>

#include "matrix/matrix.h"

Test(matrix, mat_create_empty_1)
{
    Matrix *m = mat_create_empty(3, 2);
    cr_assert_eq(mat_height(m), 3);
    cr_assert_eq(mat_width(m), 2);
    for (size_t h = 0; h < mat_height(m); h++)
    {
        for (size_t w = 0; w < mat_width(m); w++)
        {
            cr_expect_eq(mat_coef(m, h, w), 0);
        }
    }
    mat_free(m);
}

Test(matrix, mat_create_from_arr_1)
{
    Matrix *m = mat_create_from_arr(3, 2, (double[]){0, 1, 2, 3, 4, 5});
    cr_assert_eq(mat_height(m), 3);
    cr_assert_eq(mat_width(m), 2);
    for (size_t h = 0; h < mat_height(m); h++)
    {
        for (size_t w = 0; w < mat_width(m); w++)
        {
            cr_expect_float_eq(mat_coef(m, h, w), (double)(2 * h + w), 1E-3,
                               "(%zu:%zu) %lf != %lf", h, w, mat_coef(m, h, w),
                               (double)(2 * h + w));
        }
    }
    mat_free(m);
}

Test(matrix, mat_addition_1)
{
    Matrix *m1 =
        mat_create_from_arr(3, 2, (double[]){5.5, 0.1, 8.7, 9.2, 3.6, 2.0});
    Matrix *m2 =
        mat_create_from_arr(3, 2, (double[]){3.4, 5.7, 1.2, 1.1, 7.6, 0.8});
    Matrix *res = mat_addition(m1, m2);
    for (size_t h = 0; h < mat_height(res); h++)
    {
        for (size_t w = 0; w < mat_width(res); w++)
        {
            cr_expect_float_eq(mat_coef(res, h, w),
                               mat_coef(m1, h, w) + mat_coef(m2, h, w), 1E-3);
        }
    }
    mat_free(m1);
    mat_free(m2);
    mat_free(res);
}

Test(matrix, mat_addition_2)
{
    size_t height = rand() % 1000L;
    size_t width = rand() % 1000L;
    double *content1 = calloc(height * width, sizeof(double));
    double *content2 = calloc(height * width, sizeof(double));
    for (size_t i = 0; i < height * width; i++)
    {
        *(content1 + i) = (double)rand() / 10;
        *(content2 + i) = (double)rand() / 10;
    }
    Matrix *m1 = mat_create_from_arr(height, width, content1);
    Matrix *m2 = mat_create_from_arr(height, width, content2);
    free(content1);
    free(content2);
    Matrix *res = mat_addition(m1, m2);
    for (size_t h = 0; h < mat_height(res); h++)
    {
        for (size_t w = 0; w < mat_width(res); w++)
        {
            cr_expect_float_eq(mat_coef(res, h, w),
                               mat_coef(m1, h, w) + mat_coef(m2, h, w), 1E-3);
        }
    }

    mat_free(m1);
    mat_free(m2);
    mat_free(res);
}

Test(matrix, mat_normalize_1)
{
    Matrix *m = mat_create_empty(2, 1);
    *mat_coef_ptr(m, 0, 0) = 3.0;
    *mat_coef_ptr(m, 1, 0) = 1.0;

    mat_inplace_normalize(m);

    double sum = 0.0;
    for (size_t h = 0; h < mat_height(m); h++)
    {
        for (size_t w = 0; w < mat_width(m); w++)
        {
            double coef = mat_coef(m, h, w);
            cr_expect(0.0 <= coef && coef <= 1.0,
                      "Coef %lf at (h:%zu,w:%zu) is not between 0 and 1.", coef,
                      h, w);
            sum += coef;
        }
    }

    cr_expect_float_eq(sum, 1.0, 1E-3);

    mat_free(m);
}

Test(matrix, mat_normalize_2)
{
    Matrix *m = mat_create_empty(2, 1);
    *mat_coef_ptr(m, 0, 0) = 3.0;
    *mat_coef_ptr(m, 1, 0) = 1.0;

    mat_inplace_normalize(m);

    cr_expect_float_eq(mat_coef(m, 0, 0), 3.0 / 4.0, 1E-6);
    cr_expect_float_eq(mat_coef(m, 1, 0), 1.0 / 4.0, 1E-6);

    mat_free(m);
}

Test(matrix, mat_normalize_3, .exit_code = 1)
{
    Matrix *m = mat_create_empty(4, 6);
    mat_inplace_normalize(m);
    mat_free(m);
}
