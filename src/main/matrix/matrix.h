#ifndef MATIRX_H
#define MATIRX_H

typedef struct Matrix Matrix;

size_t mat_height(Matrix *m);

size_t mat_width(Matrix *m);

Matrix *mat_create_empty(size_t height, size_t width);

Matrix *mat_create_from_arr(size_t height, size_t width,
                      double *content);

void mat_free(Matrix *matrix);

double *mat_coef_addr(Matrix *m, size_t h, size_t w);

double mat_coef(Matrix *m, size_t h, size_t w);

Matrix *mat_addition(Matrix *a, Matrix *b);

Matrix *mat_scalar_multiplication(Matrix *mat, double a);

Matrix *mat_multiplication(Matrix *a, Matrix *b);

Matrix *mat_sigmoid(Matrix *m);

#endif