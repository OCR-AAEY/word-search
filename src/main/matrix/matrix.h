#ifndef MATIRX_H
#define MATIRX_H

#include <stdlib.h>

typedef struct Matrix Matrix;

size_t mat_height(const Matrix *m);

size_t mat_width(const Matrix *m);

Matrix *mat_create_empty(size_t height, size_t width);

Matrix *mat_create_from_arr(size_t height, size_t width, double *content);

void mat_free(Matrix *matrix);

Matrix *mat_deepcopy(const Matrix *m);

double *mat_coef_addr(const Matrix *m, size_t h, size_t w);

double mat_coef(const Matrix *m, size_t h, size_t w);

Matrix *mat_addition(const Matrix *a, const Matrix *b);

Matrix *mat_scalar_multiplication(Matrix *mat, double a);

Matrix *mat_multiplication(const Matrix *a, const Matrix *b);

Matrix *mat_sigmoid(const Matrix *m);

void mat_print(const Matrix *m, int precision);

#endif