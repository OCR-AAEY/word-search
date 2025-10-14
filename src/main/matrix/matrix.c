#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"

struct Matrix
{
    size_t height;
    size_t width;
    double *content;
};

size_t mat_height(Matrix *m) { return m->height; }

size_t mat_width(Matrix *m) { return m->width; }

Matrix *mat_create_empty(size_t height, size_t width)
{
    if (height == 0)
        errx(1,
             "Failed to create matrix: invalid height '%zu'. The height "
             "should be non zero.",
             height);
    if (width == 0)
        errx(1,
             "Failed to create matrix: invalid width '%zu'. The width "
             "should be non zero.",
             width);

    double *content = calloc(height * width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory with calloc.");

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory with malloc.");

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

/// @brief 
/// @param height 
/// @param width 
/// @param content allocated on the heap
/// @return 
Matrix *mat_create_from_arr(size_t height, size_t width, double *content)
{
    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory with calloc.");
    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

void mat_free(Matrix *matrix)
{
    free(matrix->content);
    free(matrix);
}

Matrix *mat_deepcopy(Matrix*m){
    double *content = calloc(m->height * m->width, sizeof(double));
    if (content == NULL)
        errx(1, "Failed to allocate memory with calloc.");

    Matrix *new_m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(1, "Failed to allocate memory with calloc.");
    *new_m = (Matrix){.height = m->height, .width = m->width, .content = content};
    return m;
}

double *mat_coef_addr(Matrix *m, size_t h, size_t w)
{
    return m->content + h * m->width + w;
}

double mat_coef(Matrix *m, size_t h, size_t w)
{
    return *mat_coef_addr(m, h, w);
}

Matrix *mat_addition(Matrix *a, Matrix *b)
{
    if (a->height != b->height)
        errx(1, "wrong height"); // todo
    if (a->width != b->width)
        errx(1, "wrong width"); // todo

    double *content = calloc(a->height * a->width, sizeof(double));
    for (size_t i = 0; i < a->height * a->width; i++)
    {
        *(content + i) = *(a->content + i) + *(b->content + i);
    }

    return mat_create_from_arr(a->height, a->width, content);
}

/// @brief Multiplies in place every coefficient of a matrix by a scalar value.
/// @param[in,out] mat Pointer to the matrix to be modified.
/// @param[in] a The scalar value to multiply with each matrix element.
/// @return Pointer to the modified matrix (same as input).
Matrix *mat_scalar_multiplication(Matrix *mat, double a)
{
    size_t height = mat_height(mat);
    size_t width = mat_width(mat);
    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double *cell = mat_coef_addr(mat, h, w);
            *cell = a * *cell;
        }
    }
    return mat;
}

Matrix *mat_multiplication(Matrix *a, Matrix *b)
{
    if (a->width != b->height)
        errx(1, "Cannot multiply matrices"); // todo

    Matrix *m = mat_create_empty(a->height, b->width);

    for (size_t h = 0; h < m->height; h++)
    {
        for (size_t w = 0; w < m->width; w++)
        {
            for (size_t i = 0; i < a->width; i++)
            {
                *mat_coef_addr(m, h, w) +=
                    mat_coef(a, h, i) * mat_coef(b, i, w);
            }
        }
    }

    return m;
}

Matrix *mat_sigmoid(Matrix *m)
{
    Matrix *res = mat_create_empty(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = 1.0 / (1.0 * exp(-*(m->content + i)));
    }

    return res;
}

Matrix* mat_map(Matrix* m, double (*f)(double)) {
    Matrix *res = mat_create_empty(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        *(res->content + i) = f(*(m->content + i));
    }

    return res;
}