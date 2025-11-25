#include <criterion/criterion.h>
#include <math.h>
#include <stdio.h>

#include "matrix/matrix.h"
#include "test_settings.h"

Test(matrix, mat_create_test)
{
    Matrix *m = mat_create_filled(16, 32, 6.626E-34);
    cr_assert_eq(mat_height(m), 16);
    cr_assert_eq(mat_width(m), 32);

    float *content = mat_coef_ptr(m, 0, 0);
    for (size_t i = 0; i < mat_height(m) * mat_width(m); i++)
    {
        cr_assert_float_eq(content[i], 6.626E-34, EPSILON);
    }

    mat_free(m);
}

Test(matrix, mat_create_zero_test)
{
    Matrix *m = mat_create_zero(45, 90);
    cr_assert_eq(mat_height(m), 45);
    cr_assert_eq(mat_width(m), 90);

    float *content = mat_coef_ptr(m, 0, 0);
    for (size_t i = 0; i < mat_height(m) * mat_width(m); i++)
    {
        cr_assert_eq(content[i], 0);
    }

    mat_free(m);
}

Test(matrix, mat_create_from_arr_test)
{
    Matrix *m = mat_create_from_arr(3, 2, (float[]){0, 1, 2, 3, 4, 5});
    cr_assert_eq(mat_height(m), 3);
    cr_assert_eq(mat_width(m), 2);

    for (size_t h = 0; h < mat_height(m); h++)
    {
        for (size_t w = 0; w < mat_width(m); w++)
        {
            cr_expect_eq(mat_coef(m, h, w), (float)(2 * h + w));
        }
    }

    mat_free(m);
}

Test(matrix, mat_addition_test)
{
    Matrix *m1 =
        mat_create_from_arr(3, 2, (float[]){5.5, 0.1, 8.7, 9.2, 3.6, 2.0});
    Matrix *m2 =
        mat_create_from_arr(3, 2, (float[]){3.4, 5.7, 1.2, 1.1, 7.6, 0.8});

    Matrix *res = mat_addition(m1, m2);

    cr_assert_eq(mat_height(res), 3);
    cr_assert_eq(mat_width(res), 2);

    for (size_t h = 0; h < mat_height(res); h++)
    {
        for (size_t w = 0; w < mat_width(res); w++)
        {
            cr_assert_float_eq(mat_coef(res, h, w),
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

        Matrix *m1 = mat_create_random_uniform(height, width, -1E10f, 1E10f);
        Matrix *m2 = mat_create_random_uniform(height, width, -1E10f, 1E10f);

        Matrix *res = mat_addition(m1, m2);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < mat_height(res); h++)
        {
            for (size_t w = 0; w < mat_width(res); w++)
            {
                cr_assert_float_eq(mat_coef(res, h, w),
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

        Matrix *m1 = mat_create_random_uniform(height, width, -1E10f, 1E10f);
        Matrix *m2 = mat_create_random_uniform(height, width, -1E10f, 1E10f);

        Matrix *res = mat_subtraction(m1, m2);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < mat_height(res); h++)
        {
            for (size_t w = 0; w < mat_width(res); w++)
            {
                cr_assert_float_eq(mat_coef(res, h, w),
                                   mat_coef(m1, h, w) - mat_coef(m2, h, w),
                                   EPSILON);
            }
        }

        mat_free(m1);
        mat_free(m2);
        mat_free(res);
    }
}

Test(matrix, mat_scalar_multiplication_random_test)
{
    REPEAT
    {
        size_t height = rand() % 1000L + 500L;
        size_t width = rand() % 1000L + 500L;

        Matrix *m = mat_create_random_uniform(height, width, -1E5f, 1E5f);
        float alpha = 10.0f * ((float)rand() / RAND_MAX);

        Matrix *res = mat_scalar_multiplication(m, alpha);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < mat_height(res); h++)
        {
            for (size_t w = 0; w < mat_width(res); w++)
            {
                float expected = alpha * mat_coef(m, h, w);
                cr_assert_float_eq(mat_coef(res, h, w), expected, EPSILON);
            }
        }

        mat_free(m);
        mat_free(res);
    }
}

Test(matrix, mat_multiplication_random_test)
{
    REPEAT
    {
        size_t l_dim = rand() % 200 + 50;
        size_t m_dim = rand() % 200 + 50;
        size_t r_dim = rand() % 200 + 50;

        Matrix *m1 = mat_create_random_uniform(l_dim, m_dim, -1E3f, 1E3f);
        Matrix *m2 = mat_create_random_uniform(m_dim, r_dim, -1E3f, 1E3f);

        Matrix *res = mat_multiplication(m1, m2);

        cr_assert_eq(mat_height(res), l_dim);
        cr_assert_eq(mat_width(res), r_dim);

        for (size_t i = 0; i < l_dim; i++)
        {
            for (size_t j = 0; j < r_dim; j++)
            {
                float expected = 0.0f;
                for (size_t k = 0; k < m_dim; k++)
                {
                    expected += mat_coef(m1, i, k) * mat_coef(m2, k, j);
                }

                cr_assert_float_eq(mat_coef(res, i, j), expected, EPSILON,
                                   "expected %f but got %f", expected,
                                   mat_coef(res, i, j));
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

        Matrix *m1 = mat_create_random_uniform(height, width, -1E5f, 1E5f);
        Matrix *m2 = mat_create_random_uniform(height, width, -1E5f, 1E5f);

        Matrix *res = mat_hadamard(m1, m2);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < mat_height(res); h++)
        {
            for (size_t w = 0; w < mat_width(res); w++)
            {
                float expected = mat_coef(m1, h, w) * mat_coef(m2, h, w);
                cr_assert_float_eq(mat_coef(res, h, w), expected, 1.0f,
                                   "%.2f * %.2f = %.2f != %.2f",
                                   mat_coef(m1, h, w), mat_coef(m2, h, w),
                                   expected, mat_coef(res, h, w));
            }
        }

        mat_free(m1);
        mat_free(m2);
        mat_free(res);
    }
}

Test(matrix, mat_relu_random_test)
{
    REPEAT
    {
        size_t height = rand() % 1000 + 500;
        size_t width = rand() % 1000 + 500;

        Matrix *m = mat_create_random_uniform(height, width, -1E5f, 1E5f);
        Matrix *res = mat_relu(m);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < height; h++)
        {
            for (size_t w = 0; w < width; w++)
            {
                float x = mat_coef(m, h, w);
                float expected = x > 0.0f ? x : 0.0f;

                cr_assert_float_eq(mat_coef(res, h, w), expected, EPSILON);
            }
        }

        mat_free(m);
        mat_free(res);
    }
}

Test(matrix, mat_relu_derivative_random_test)
{
    REPEAT
    {
        size_t height = rand() % 1000 + 500;
        size_t width = rand() % 1000 + 500;

        Matrix *m = mat_create_random_uniform(height, width, -1E5f, 1E5f);
        Matrix *res = mat_relu_derivative(m);

        cr_assert_eq(mat_height(res), height);
        cr_assert_eq(mat_width(res), width);

        for (size_t h = 0; h < height; h++)
        {
            for (size_t w = 0; w < width; w++)
            {
                float x = mat_coef(m, h, w);
                float expected = x > 0.0f ? 1.0f : 0.0f;

                cr_assert_float_eq(mat_coef(res, h, w), expected, EPSILON);
            }
        }

        mat_free(m);
        mat_free(res);
    }
}

Test(matrix, mat_inplace_softmax_random_test)
{
    REPEAT
    {
        size_t height = rand() % 500 + 50;
        size_t width = rand() % 500 + 50;

        Matrix *m = mat_create_random_uniform(height, width, -10, 10);
        Matrix *original = mat_deepcopy(m);

        float sum = 0.0f;
        for (size_t h = 0; h < height; h++)
        {
            for (size_t w = 0; w < width; w++)
            {
                *mat_coef_ptr(original, h, w) = expf(mat_coef(original, h, w));
                sum += mat_coef(original, h, w);
            }
        }
        for (size_t h = 0; h < height; h++)
        {
            for (size_t w = 0; w < width; w++)
            {
                *mat_coef_ptr(original, h, w) /= sum;
            }
        }

        mat_inplace_softmax(m);

        for (size_t h = 0; h < height; h++)
        {
            for (size_t w = 0; w < width; w++)
            {
                cr_assert_float_eq(mat_coef(m, h, w), mat_coef(original, h, w),
                                   1E-3f, "%f, %f", mat_coef(m, h, w),
                                   mat_coef(original, h, w));
            }
        }

        mat_free(m);
        mat_free(original);
    }
}

Test(matrix, mat_strip_margins_test_1)
{
    Matrix *m = mat_create_zero(5, 5);
    for (size_t h = 1; h <= 3; h++)
        for (size_t w = 1; w <= 3; w++)
            *mat_coef_ptr(m, h, w) = 1.0f;

    Matrix *expected = mat_create_filled(3, 3, 1.0f);

    Matrix *res = mat_strip_margins(m);

    cr_assert_eq(mat_height(res), 3);
    cr_assert_eq(mat_width(res), 3);

    for (size_t h = 0; h < 3; h++)
    {
        for (size_t w = 0; w < 3; w++)
        {
            cr_assert_float_eq(mat_coef(res, h, w), mat_coef(expected, h, w),
                               EPSILON);
        }
    }

    mat_free(m);
    mat_free(res);
    mat_free(expected);
}

Test(matrix, mat_strip_margins_test_2)
{
    Matrix *m = mat_create_zero(5, 5);
    *mat_coef_ptr(m, 1, 1) = 1.0f;
    *mat_coef_ptr(m, 3, 3) = 1.0f;

    Matrix *expected = mat_create_zero(3, 3);
    *mat_coef_ptr(expected, 0, 0) = 1.0f;
    *mat_coef_ptr(expected, 2, 2) = 1.0f;

    Matrix *res = mat_strip_margins(m);

    cr_assert_eq(mat_height(res), 3);
    cr_assert_eq(mat_width(res), 3);

    for (size_t h = 0; h < 3; h++)
    {
        for (size_t w = 0; w < 3; w++)
        {
            cr_assert_float_eq(mat_coef(res, h, w), mat_coef(expected, h, w),
                               EPSILON);
        }
    }

    mat_free(m);
    mat_free(res);
    mat_free(expected);
}

Test(matrix, mat_strip_margins_test_3)
{
    Matrix *m = mat_create_zero(5, 5);
    *mat_coef_ptr(m, 0, 0) = 1.0f;
    *mat_coef_ptr(m, 2, 3) = 1.0f;

    Matrix *expected = mat_create_zero(3, 4);
    *mat_coef_ptr(expected, 0, 0) = 1.0f;
    *mat_coef_ptr(expected, 2, 3) = 1.0f;

    Matrix *res = mat_strip_margins(m);

    cr_assert_eq(mat_height(res), 3);
    cr_assert_eq(mat_width(res), 4);

    for (size_t h = 0; h < 3; h++)
    {
        for (size_t w = 0; w < 4; w++)
        {
            cr_assert_float_eq(mat_coef(res, h, w), mat_coef(expected, h, w),
                               EPSILON);
        }
    }

    mat_free(m);
    mat_free(res);
    mat_free(expected);
}

Test(matrix, mat_strip_margins_failure_test, .exit_code = EXIT_FAILURE)
{
    Matrix *m = mat_create_zero(5, 5);
    Matrix *res = mat_strip_margins(m);

    mat_free(m);
    mat_free(res);
}

Test(matrix, mat_normalize_random_test)
{
    REPEAT
    {
        size_t height = rand() % 1000L + 500L;
        size_t width = rand() % 1000L + 500L;

        Matrix *m = mat_create_random_uniform(height, width, 0.0f, 1E10f);

        mat_inplace_normalize(m);

        cr_assert_eq(mat_height(m), height);
        cr_assert_eq(mat_width(m), width);

        float sum = 0.0;

        for (size_t h = 0; h < mat_height(m); h++)
        {
            for (size_t w = 0; w < mat_width(m); w++)
            {
                sum += mat_coef(m, h, w);
            }
        }

        cr_assert_float_eq(sum, 1.0f, 1E-2f, "exp:%f and got:%f", sum, 1.0f);

        mat_free(m);
    }
}

Test(matrix, mat_normalize_failure_test, .exit_code = EXIT_FAILURE)
{
    Matrix *m = mat_create_zero(4, 6);

    mat_inplace_normalize(m);

    mat_free(m);
}

Test(matrix, mat_load_from_file_test)
{
    float arr[9] = {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    Matrix *expected = mat_create_from_arr(3, 3, arr);

    Matrix *m = mat_load_from_file("./src/test/matrix/test_load_1.matrix");

    for (size_t h = 0; h < mat_height(m); h++)
    {
        for (size_t w = 0; w < mat_width(m); w++)
        {
            cr_assert_float_eq(mat_coef(m, h, w), mat_coef(expected, h, w),
                               EPSILON, "expected %.2f but got %.2f",
                               mat_coef(expected, h, w), mat_coef(m, h, w));
        }
    }

    mat_free(m);
    mat_free(expected);
}

Test(matrix, mat_save_and_load_test)
{
    float arr[9] = {0.0f, -1.0f, 2.0f, -3.0f, 4.0f, -5.0f, 6.0f, -7.0f, 8.0f};
    Matrix *original = mat_create_from_arr(3, 3, arr);

    mat_save_to_file(original, "./save_and_load_test.matrix");
    Matrix *m = mat_load_from_file("./save_and_load_test.matrix");

    cr_assert_eq(mat_height(m), 3);
    cr_assert_eq(mat_width(m), 3);

    for (size_t h = 0; h < mat_height(m); h++)
    {
        for (size_t w = 0; w < mat_width(m); w++)
        {
            cr_assert_float_eq(mat_coef(m, h, w), mat_coef(original, h, w),
                               EPSILON, "expected %.2f but got %.2f",
                               mat_coef(original, h, w), mat_coef(m, h, w));
        }
    }

    mat_free(m);
    mat_free(original);
}

Test(matrix, mat_save_and_load_random_test)
{

    REPEAT
    {
        size_t height = rand() % 200 + 50;
        size_t width = rand() % 200 + 50;

        Matrix *original =
            mat_create_random_uniform(height, width, -1E3f, 1E3f);

        mat_save_to_file(original, "./save_and_load_random_test.matrix");
        Matrix *m = mat_load_from_file("./save_and_load_random_test.matrix");

        cr_assert_eq(mat_height(m), mat_height(original));
        cr_assert_eq(mat_width(m), mat_width(original));

        for (size_t h = 0; h < mat_height(m); h++)
        {
            for (size_t w = 0; w < mat_width(m); w++)
            {
                cr_assert_float_eq(mat_coef(m, h, w), mat_coef(original, h, w),
                                   EPSILON, "expected %.2f but got %.2f",
                                   mat_coef(original, h, w), mat_coef(m, h, w));
            }
        }

        mat_free(m);
        mat_free(original);
    }
}
