#include <criterion/criterion.h>
#include <stdio.h>

#include "matrix/matrix.h"
#include "test_settings.h"

Test(matrix, mat_create_test)
{
    Matrix *m = mat_create(16, 32, 6.626E-34);
    cr_assert_eq(mat_height(m), 16);
    cr_assert_eq(mat_width(m), 32);

    double *content = mat_unsafe_coef_ptr(m, 0, 0);
    for (size_t i = 0; i < mat_height(m) * mat_width(m); i++)
    {
        cr_expect_float_eq(content[i], 6.626E-34, EPSILON);
    }

    mat_free(m);
}

Test(matrix, mat_create_zero_test)
{
    Matrix *m = mat_create_zero(45, 90);
    cr_assert_eq(mat_height(m), 45);
    cr_assert_eq(mat_width(m), 90);

    double *content = mat_unsafe_coef_ptr(m, 0, 0);
    for (size_t i = 0; i < mat_height(m) * mat_width(m); i++)
    {
        cr_expect_eq(content[i], 0);
    }

    mat_free(m);
}

Test(matrix, mat_create_from_arr_test)
{
    Matrix *m = mat_create_from_arr(3, 2, (double[]){0, 1, 2, 3, 4, 5});
    cr_assert_eq(mat_height(m), 3);
    cr_assert_eq(mat_width(m), 2);

    for (size_t h = 0; h < mat_height(m); h++)
    {
        for (size_t w = 0; w < mat_width(m); w++)
        {
            cr_expect_eq(mat_coef(m, h, w), (double)(2 * h + w));
        }
    }

    mat_free(m);
}

Test(matrix, mat_addition_test)
{
    Matrix *m1 =
        mat_create_from_arr(3, 2, (double[]){5.5, 0.1, 8.7, 9.2, 3.6, 2.0});
    Matrix *m2 =
        mat_create_from_arr(3, 2, (double[]){3.4, 5.7, 1.2, 1.1, 7.6, 0.8});

    Matrix *res = mat_addition(m1, m2);

    cr_assert_eq(mat_height(res), 3);
    cr_assert_eq(mat_width(res), 2);

    for (size_t h = 0; h < mat_height(res); h++)
    {
        for (size_t w = 0; w < mat_width(res); w++)
        {
            cr_expect_float_eq(mat_coef(res, h, w),
                               mat_coef(m1, h, w) + mat_coef(m2, h, w),
                               EPSILON);
        }
    }
    mat_free(m1);
    mat_free(m2);
    mat_free(res);
}

Test(matrix, mat_addition_random_test)
{
    REPEAT
    {
        size_t height = rand() % 1000L + 500L;
        size_t width = rand() % 1000L + 500L;

        Matrix *m1 = mat_create_uniform_random(height, width, -1E100, 1E100);
        Matrix *m2 = mat_create_uniform_random(height, width, -1E100, 1E100);

        Matrix *res = mat_addition(m1, m2);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < mat_height(res); h++)
        {
            for (size_t w = 0; w < mat_width(res); w++)
            {
                cr_expect_float_eq(mat_coef(res, h, w),
                                   mat_coef(m1, h, w) + mat_coef(m2, h, w),
                                   EPSILON);
            }
        }

        mat_free(m1);
        mat_free(m2);
        mat_free(res);
    }
}

Test(matrix, mat_subtraction_random_test)
{
    REPEAT
    {
        size_t height = rand() % 1000L + 500L;
        size_t width = rand() % 1000L + 500L;

        Matrix *m1 = mat_create_uniform_random(height, width, -1E100, 1E100);
        Matrix *m2 = mat_create_uniform_random(height, width, -1E100, 1E100);

        Matrix *res = mat_subtraction(m1, m2);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < mat_height(res); h++)
        {
            for (size_t w = 0; w < mat_width(res); w++)
            {
                cr_expect_float_eq(mat_coef(res, h, w),
                                   mat_coef(m1, h, w) - mat_coef(m2, h, w),
                                   EPSILON);
            }
        }

        mat_free(m1);
        mat_free(m2);
        mat_free(res);
    }
}

Test(matrix, mat_hadamard_random_test)
{
    REPEAT
    {
        size_t height = rand() % 1000L + 500L;
        size_t width = rand() % 1000L + 500L;

        Matrix *m1 = mat_create_uniform_random(height, width, -1E100, 1E100);
        Matrix *m2 = mat_create_uniform_random(height, width, -1E100, 1E100);

        Matrix *res = mat_hadamard(m1, m2);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < mat_height(res); h++)
        {
            for (size_t w = 0; w < mat_width(res); w++)
            {
                cr_expect_float_eq(mat_coef(res, h, w),
                                   mat_coef(m1, h, w) * mat_coef(m2, h, w),
                                   EPSILON);
            }
        }

        mat_free(m1);
        mat_free(m2);
        mat_free(res);
    }
}

Test(matrix, mat_normalize_random_test)
{
    REPEAT
    {
        size_t height = rand() % 1000L + 500L;
        size_t width = rand() % 1000L + 500L;

        Matrix *m = mat_create_uniform_random(height, width, -1E100, 1E100);

        mat_inplace_normalize(m);

        cr_assert_eq(mat_height(m), height);
        cr_assert_eq(mat_width(m), width);

        double sum = 0.0;

        for (size_t h = 0; h < mat_height(m); h++)
        {
            for (size_t w = 0; w < mat_width(m); w++)
            {
                sum += mat_coef(m, h, w);
            }
        }

        cr_expect_float_eq(sum, 1.0, EPSILON);

        mat_free(m);
    }
}

Test(matrix, mat_normalize_failure, .exit_code = 1)
{
    Matrix *m = mat_create_zero(4, 6);

    mat_inplace_normalize(m);

    mat_free(m);
}
