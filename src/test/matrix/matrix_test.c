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
    double *content = calloc(6, sizeof(double));
    for (size_t i = 0; i < 6; i++)
    {
        *(content + i) = (double)i;
    }
    Matrix *m = mat_create_from_arr(3, 2, content);
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
    double *content1 = malloc(6 * sizeof(double));
    double *content2 = malloc(6 * sizeof(double));
    content1[0] = 5.5;
    content1[1] = 0.1;
    content1[2] = 8.7;
    content1[3] = 9.2;
    content1[4] = 3.6;
    content1[5] = 2.0;
    content2[0] = 3.4;
    content2[1] = 5.7;
    content2[2] = 1.2;
    content2[3] = 1.1;
    content2[4] = 7.6;
    content2[5] = 0.8;
    Matrix *m1 = mat_create_from_arr(3, 2, content1);
    Matrix *m2 = mat_create_from_arr(3, 2, content2);
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
