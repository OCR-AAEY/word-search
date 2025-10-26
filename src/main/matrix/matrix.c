#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "utils/shuffle_array.h"

/// @brief A 2D matrix of double-precision floating point numbers.
struct Matrix
{
    /// @brief Number of rows (height) of the matrix.
    size_t height;
    /// @brief Number of columns (width) of the matrix.
    size_t width;
    /// @brief The matrix elements stored in a contiguous row-major array.
    double *content;
};

inline size_t mat_height(const Matrix *m) { return m->height; }

inline size_t mat_width(const Matrix *m) { return m->width; }

Matrix *mat_create_empty(size_t height, size_t width)
{
    if (height == 0)
        errx(1,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(1,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    double *content = calloc(height * width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory for matrix content.");

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory for matrix struct.");

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

Matrix *mat_create_from_arr(size_t height, size_t width, double *content)
{
    if (height == 0)
        errx(1,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(1,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory for matrix struct.");

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

Matrix *mat_create_gaussian_random(size_t height, size_t width)
{
    if (height == 0)
        errx(1,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(1,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    double *content = calloc(height * width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory for matrix content.");

    // TODO: Implement Gaussian distribution on content
    shuffle_array(content, height * width);

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory for matrix struct.");

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

Matrix *mat_create_uniform_random(size_t height, size_t width)
{
    if (height == 0)
        errx(1,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(1,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    double *content = calloc(height * width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory for matrix content.");

    // TODO: Implement uniform distribution on content
    shuffle_array(content, height * width);

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory for matrix struct.");

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

void mat_free(Matrix *matrix)
{
    free(matrix->content);
    free(matrix);
}

Matrix *mat_deepcopy(const Matrix *m)
{
    double *content = calloc(m->height * m->width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory for deepcopy content.");

    for (size_t i = 0; i < m->height * m->width; i++)
        *(content + i) = *(m->content + i);

    Matrix *new_m = malloc(sizeof(Matrix));
    if (new_m == NULL)
        errx(1, "Failed to allocate memory for matrix struct.");

    *new_m =
        (Matrix){.height = m->height, .width = m->width, .content = content};
    return new_m;
}

double *mat_unsafe_coef_ptr(const Matrix *m, size_t h, size_t w)
{
    return m->content + h * m->width + w;
}

double *mat_coef_ptr(const Matrix *m, size_t h, size_t w)
{
    if (h >= m->height)
        errx(1, "Invalid height given. Expected < %zu and got %zu.", m->height,
             h);
    if (w >= m->width)
        errx(1, "Invalid width given. Expected < %zu and got %zu.", m->width,
             w);
    return mat_unsafe_coef_ptr(m, h, w);
}

double mat_coef(const Matrix *m, size_t h, size_t w)
{
    if (h >= m->height)
        errx(1, "Invalid height given. Expected < %zu and got %zu.", m->height,
             h);
    if (w >= m->width)
        errx(1, "Invalid width given. Expected < %zu and got %zu.", m->width,
             w);
    return *mat_unsafe_coef_ptr(m, h, w);
}

Matrix *mat_addition(const Matrix *a, const Matrix *b)
{
    if (a->height != b->height)
        errx(1, "Matrix addition failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(1, "Matrix addition failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    double *content = calloc(a->height * a->width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory for matrix addition result.");

    for (size_t i = 0; i < a->height * a->width; i++)
    {
        *(content + i) = *(a->content + i) + *(b->content + i);
    }

    return mat_create_from_arr(a->height, a->width, content);
}

void mat_inplace_addition(Matrix *a, Matrix *b)
{
    if (a->height != b->height)
        errx(1, "Matrix addition failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(1, "Matrix addition failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    for (size_t i = 0; i < a->height * a->width; i++)
    {
        *(a->content + i) += *(b->content + i);
    }
}

Matrix *mat_scalar_multiplication(const Matrix *m, double a)
{
    Matrix *res = mat_create_empty(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = *(m->content + i) * a;
    }

    return res;
}

void mat_inplace_scalar_multiplication(Matrix *m, double a)
{
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(m->content + i) *= a;
    }
}

Matrix *mat_multiplication(const Matrix *a, const Matrix *b)
{
    if (a->width != b->height)
        errx(1, "Cannot multiply two matrices if the width of the first does "
                "not match the height of the second.");

    Matrix *m = mat_create_empty(a->height, b->width);

    for (size_t h = 0; h < m->height; h++)
    {
        for (size_t w = 0; w < m->width; w++)
        {
            for (size_t i = 0; i < a->width; i++)
            {
                *mat_unsafe_coef_ptr(m, h, w) += *mat_unsafe_coef_ptr(a, h, i) *
                                                 *mat_unsafe_coef_ptr(b, i, w);
            }
        }
    }

    return m;
}

Matrix *mat_sigmoid(const Matrix *m)
{
    Matrix *res = mat_create_empty(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = 1.0 / (1.0 + exp(-*(m->content + i)));
    }

    return res;
}

void mat_inplace_sigmoid(Matrix *m)
{
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(m->content + i) = 1.0 / (1.0 + exp(-*(m->content + i)));
    }
}

void mat_inplace_normalize(Matrix *m)
{
    double sum = 0.0;

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        sum += *(m->content + i);
    }

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(m->content + i) /= sum;
    }
}

Matrix *mat_vertical_flatten(const Matrix *m)
{
    Matrix *res = mat_create_empty(1, m->height * m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = *(m->content + i);
    }

    return res;
}

Matrix *mat_horizontal_flatten(const Matrix *m)
{
    Matrix *res = mat_create_empty(m->height * m->width, 1);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = *(m->content + i);
    }

    return res;
}

Matrix *mat_map(const Matrix *m, double (*f)(double))
{
    Matrix *res = mat_create_empty(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = f(*(m->content + i));
    }

    return res;
}

Matrix *mat_map_with_indexes(const Matrix *m,
                             double (*f)(double, size_t, size_t))
{
    Matrix *res = mat_create_empty(m->height, m->width);

    for (size_t h = 0; h < m->height; h++)
    {
        for (size_t w = 0; w < m->width; w++)
            *mat_unsafe_coef_ptr(res, h, w) =
                f(*mat_unsafe_coef_ptr(m, h, w), h, w);
    }

    return res;
}

void mat_print(const Matrix *m, int precision)
{
    if (m == NULL)
    {
        errx(1, "Given matrix pointer is null.");
    }
    else
    {
        char fmt[16];
        snprintf(fmt, sizeof(fmt), "%%.%df", precision);

        for (size_t h = 0; h < m->height; h++)
        {
            for (size_t w = 0; w < m->width; w++)
            {
                printf(fmt, mat_coef(m, h, w));
                if (w < m->width - 1)
                    printf("  ");
            }
            printf("\n");
        }
    }
}
