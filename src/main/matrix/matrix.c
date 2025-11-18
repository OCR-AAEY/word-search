#include <err.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "matrix.h"
#include "utils/math/gcd.h"
#include "utils/math/sigmoid.h"
#include "utils/random/random.h"

/// @brief A 2D matrix of single-precision floating point numbers.
struct Matrix
{
    /// @brief Number of rows (height) of the matrix.
    size_t height;
    /// @brief Number of columns (width) of the matrix.
    size_t width;
    /// @brief The matrix elements stored in a contiguous row-major array.
    float *content;
};

inline size_t mat_height(const Matrix *m) { return m->height; }

inline size_t mat_width(const Matrix *m) { return m->width; }

Matrix *mat_create(size_t height, size_t width, float value)
{
    if (height == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    float *content = calloc(height * width, sizeof(float));
    if (content == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix content.");

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix struct.");

    for (size_t i = 0; i < height * width; i++)
        content[i] = value;

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

Matrix *mat_create_zero(size_t height, size_t width)
{
    if (height == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    float *content = calloc(height * width, sizeof(float));
    if (content == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix content.");

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix struct.");

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

Matrix *mat_create_from_arr(size_t height, size_t width, const float *content)
{
    if (height == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix struct.");

    float *content_copy = calloc(height * width, sizeof(float));
    if (content_copy == NULL)
        errx(EXIT_FAILURE,
             "Failed to allocate memory for matrix struct's content.");

    for (size_t i = 0; i < height * width; i++)
    {
        content_copy[i] = content[i];
    }

    *m = (Matrix){.height = height, .width = width, .content = content_copy};
    return m;
}

Matrix *mat_create_uniform_random(size_t height, size_t width, float min,
                                  float max)
{
    if (height == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    float *content = calloc(height * width, sizeof(float));
    if (content == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix content.");

    for (size_t i = 0; i < height * width; i++)
    {
        content[i] = rand_f_uniform_nm(min, max);
    }

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
    {
        free(content);
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix struct.");
    }

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

Matrix *mat_create_gaussian_random(size_t height, size_t width)
{
    if (height == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    float *content = calloc(height * width, sizeof(float));
    if (content == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix content.");

    for (size_t i = 0; i < height * width; i++)
    {
        content[i] = rand_f_gaussian();
    }

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
    {
        free(content);
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix struct.");
    }

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

Matrix *mat_create_normal_random(size_t height, size_t width, float mean,
                                 float stddev)
{
    if (height == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid height '%zu'. Height must be "
             "non-zero.",
             height);
    if (width == 0)
        errx(EXIT_FAILURE,
             "Failed to create matrix: invalid width '%zu'. Width must be "
             "non-zero.",
             width);

    float *content = calloc(height * width, sizeof(float));
    if (content == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix content.");

    for (size_t i = 0; i < height * width; i++)
    {
        content[i] = rand_d_normal(mean, stddev);
    }

    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
    {
        free(content);
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix struct.");
    }

    *m = (Matrix){.height = height, .width = width, .content = content};
    return m;
}

void mat_free(Matrix *matrix)
{
    free(matrix->content);
    free(matrix);
}

void mat_free_matrix_array(Matrix **array, size_t lentgh)
{
    for (size_t i = 0; i < lentgh; i++)
    {
        if (array[i] != NULL)
            mat_free(array[i]);
    }
    free(array);
}

int mat_eq(Matrix *a, Matrix *b)
{
    if (a == b)
        return 1;

    if (a->height != b->height || a->width != b->width)
        return 0;

    float epsilon = 1E-9;
    for (size_t i = 0; i < a->height * a->width; i++)
    {
        if (fabs(a->content[i] - b->content[i]) > epsilon)
            return 0;
    }

    return 1;
}

Matrix *mat_deepcopy(const Matrix *m)
{
    float *content = calloc(m->height * m->width, sizeof(float));
    if (content == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for deepcopy content.");

    for (size_t i = 0; i < m->height * m->width; i++)
        content[i] = m->content[i];

    Matrix *new_m = malloc(sizeof(Matrix));
    if (new_m == NULL)
        errx(EXIT_FAILURE, "Failed to allocate memory for matrix struct.");

    *new_m =
        (Matrix){.height = m->height, .width = m->width, .content = content};
    return new_m;
}

float *mat_unsafe_coef_ptr(const Matrix *m, size_t h, size_t w)
{
    return m->content + h * m->width + w;
}

float *mat_coef_ptr(const Matrix *m, size_t h, size_t w)
{
    if (h >= m->height)
        errx(EXIT_FAILURE, "Invalid height given. Expected < %zu and got %zu.",
             m->height, h);
    if (w >= m->width)
        errx(EXIT_FAILURE, "Invalid width given. Expected < %zu and got %zu.",
             m->width, w);
    return mat_unsafe_coef_ptr(m, h, w);
}

float mat_coef(const Matrix *m, size_t h, size_t w)
{
    if (h >= m->height)
        errx(EXIT_FAILURE, "Invalid height given. Expected < %zu and got %zu.",
             m->height, h);
    if (w >= m->width)
        errx(EXIT_FAILURE, "Invalid width given. Expected < %zu and got %zu.",
             m->width, w);
    return *mat_unsafe_coef_ptr(m, h, w);
}

Matrix *mat_addition(const Matrix *a, const Matrix *b)
{
    if (a->height != b->height)
        errx(EXIT_FAILURE,
             "Matrix addition failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(EXIT_FAILURE,
             "Matrix addition failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    Matrix *res = mat_deepcopy(a);

    for (size_t i = 0; i < res->height * res->width; i++)
    {
        res->content[i] += b->content[i];
    }

    return res;
}

void mat_inplace_addition(Matrix *a, const Matrix *b)
{
    if (a->height != b->height)
        errx(EXIT_FAILURE,
             "Matrix addition failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(EXIT_FAILURE,
             "Matrix addition failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    for (size_t i = 0; i < a->height * a->width; i++)
    {
        a->content[i] += b->content[i];
    }
}

Matrix *mat_subtraction(const Matrix *a, const Matrix *b)
{
    if (a->height != b->height)
        errx(EXIT_FAILURE,
             "Matrix subtraction failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(EXIT_FAILURE,
             "Matrix subtraction failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    Matrix *res = mat_deepcopy(a);

    for (size_t i = 0; i < res->height * res->width; i++)
    {
        res->content[i] -= b->content[i];
    }

    return res;
}

void mat_inplace_subtraction(Matrix *a, const Matrix *b)
{
    if (a->height != b->height)
        errx(EXIT_FAILURE,
             "Matrix subtraction failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(EXIT_FAILURE,
             "Matrix subtraction failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    for (size_t i = 0; i < a->height * a->width; i++)
    {
        a->content[i] -= b->content[i];
    }
}

Matrix *mat_scalar_multiplication(const Matrix *m, float a)
{
    Matrix *res = mat_create_zero(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        res->content[i] = m->content[i] * a;
    }

    return res;
}

void mat_inplace_scalar_multiplication(Matrix *m, float a)
{
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        m->content[i] *= a;
    }
}

Matrix *mat_multiplication(const Matrix *a, const Matrix *b)
{
    if (a->width != b->height)
        errx(EXIT_FAILURE,
             "Cannot multiply two matrices if the width of the first does "
             "not match the height of the second.");

    Matrix *m = mat_create_zero(a->height, b->width);

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

Matrix *mat_hadamard(const Matrix *a, const Matrix *b)
{
    if (a->height != b->height)
        errx(EXIT_FAILURE,
             "Matrix hadamard product failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(EXIT_FAILURE,
             "Matrix hadamard product failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    Matrix *res = mat_create_zero(a->height, a->width);

    for (size_t i = 0; i < a->height * a->width; i++)
    {
        res->content[i] = a->content[i] * b->content[i];
    }

    return res;
}

void mat_inplace_hadamard(Matrix *a, const Matrix *b)
{
    if (a->height != b->height)
        errx(EXIT_FAILURE,
             "Matrix hadamard product failed: mismatched heights (%zu vs %zu).",
             a->height, b->height);
    if (a->width != b->width)
        errx(EXIT_FAILURE,
             "Matrix hadamard product failed: mismatched widths (%zu vs %zu).",
             a->width, b->width);

    for (size_t i = 0; i < a->height * a->width; i++)
    {
        a->content[i] *= b->content[i];
    }
}

Matrix *mat_sigmoid(const Matrix *m)
{
    Matrix *res = mat_create_zero(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        res->content[i] = sigmoid(m->content[i]);
    }

    return res;
}

void mat_inplace_sigmoid(Matrix *m)
{
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        m->content[i] = sigmoid(m->content[i]);
    }
}

Matrix *mat_sigmoid_derivative(const Matrix *m)
{
    Matrix *res = mat_create_zero(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        res->content[i] = sigmoid_derivative(m->content[i]);
    }

    return res;
}

void mat_inplace_sigmoid_derivative(Matrix *m)
{
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        m->content[i] = sigmoid_derivative(m->content[i]);
    }
}

void mat_inplace_relu(Matrix *m)
{
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        if (m->content[i] < 0)
            m->content[i] = 0;
    }
}

Matrix *mat_relu_derivative(Matrix *m)
{
    Matrix *res = mat_create_zero(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        if (m->content[i] > 0)
            res->content[i] = 1;
        else
            res->content[i] = 0;
    }

    return res;
}

void mat_inplace_softmax(Matrix *m)
{
    float sum = 0.0f;
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        m->content[i] = expf(m->content[i]);
        sum += m->content[i];
    }
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        m->content[i] /= sum;
    }
}

Matrix *mat_strip_margins(Matrix *m)
{
    // Whether an activated (<=> black <=> 1.0f) pixel has been found.
    int found;
    // The new coordinates (included).
    size_t h_i = 0, h_f = m->height - 1, w_i = 0, w_f = m->width - 1;

    found = 0;
    while (h_i < m->height && !found)
    {
        for (size_t w = 0; w < m->width && !found; w++)
            found = *mat_coef_ptr(m, h_i, w) > 0.5f;
        h_i++;
    }
    h_i--;

    found = 0;
    while (h_f > 0 && !found)
    {
        for (size_t w = 0; w < m->width && !found; w++)
            found = *mat_coef_ptr(m, h_f, w) > 0.5f;
        h_f--;
    }
    h_f++;

    found = 0;
    while (w_i < m->width && !found)
    {
        for (size_t h = 0; h < m->height && !found; h++)
            found = *mat_coef_ptr(m, h, w_i) > 0.5f;
        w_i++;
    }
    w_i--;

    found = 0;
    while (w_f > 0 && !found)
    {
        for (size_t h = 0; h < m->height && !found; h++)
            found = *mat_coef_ptr(m, h, w_f) > 0.5f;
        w_f--;
    }
    w_f++;

    if (h_i > h_f || w_i > w_f)
        errx(EXIT_FAILURE, "Matrix empty.");

    Matrix *res = mat_create_zero(h_f - h_i + 1, w_f - w_i + 1);
    for (size_t h = 0; h < res->height; h++)
        for (size_t w = 0; w < res->width; w++)
            *mat_coef_ptr(res, h, w) = *mat_coef_ptr(m, h_i + h, w_i + w);

    return res;
}

Matrix *mat_scale_to_28(Matrix *m)
{
    const size_t TARGET = 28;
    Matrix *res = mat_create_zero(TARGET, TARGET);

    // Compute scaling factors for height and width
    float scale_h = (float)m->height / (float)TARGET;
    float scale_w = (float)m->width / (float)TARGET;

    // Use the larger scale to preserve aspect ratio
    float factor = (scale_h > scale_w) ? scale_h : scale_w;

    // Compute offset to center the content
    float h_offset = ((TARGET * factor) - m->height) / 2.0f;
    float w_offset = ((TARGET * factor) - m->width) / 2.0f;

    for (size_t h = 0; h < TARGET; h++)
    {
        for (size_t w = 0; w < TARGET; w++)
        {
            // Map target pixel to source matrix coordinates
            float sh = h * factor - h_offset;
            float sw = w * factor - w_offset;

            // Clamp to valid source range
            if (sh < 0)
                sh = 0;
            if (sw < 0)
                sw = 0;
            if (sh > m->height - 1)
                sh = m->height - 1;
            if (sw > m->width - 1)
                sw = m->width - 1;

            // Split into integer and fractional parts
            float sh_frac, sw_frac, sh_int, sw_int;
            sh_frac = modff(sh, &sh_int);
            sw_frac = modff(sw, &sw_int);

            size_t ih = (size_t)sh_int;
            size_t iw = (size_t)sw_int;

            // Neighboring indices, clamped
            size_t ih2 = (ih + 1 < m->height) ? ih + 1 : ih;
            size_t iw2 = (iw + 1 < m->width) ? iw + 1 : iw;

            // Bilinear interpolation weights
            float w_tl = (1.0f - sh_frac) * (1.0f - sw_frac);
            float w_tr = (1.0f - sh_frac) * sw_frac;
            float w_bl = sh_frac * (1.0f - sw_frac);
            float w_br = sh_frac * sw_frac;

            // Compute interpolated pixel value
            float pixel =
                w_tl * mat_coef(m, ih, iw) + w_tr * mat_coef(m, ih, iw2) +
                w_bl * mat_coef(m, ih2, iw) + w_br * mat_coef(m, ih2, iw2);

            *mat_unsafe_coef_ptr(res, h, w) = roundf(pixel);
        }
    }

    return res;
}

// Matrix *mat_scale_to_28(Matrix *m)
// {
//     Matrix *res = mat_create_zero(28, 28);

//     float factor;
//     size_t h0 = 0, w0 = 0;
//     if (m->height > m->width)
//     {
//         factor = (float)(m->height - 1) / 27.0f;
//         w0 = (m->height - m->width) / 2;
//     }
//     else if (m->width > m->height)
//     {
//         factor = (float)(m->width - 1) / 27.0f;
//         h0 = (m->width - m->height) / 2;
//     }
//     else
//     {
//         factor = (float)(m->height - 1) / 27.0f;
//     }

//     for (size_t h = 0; h < 28; h++)
//     {
//         for (size_t w = 0; w < 28; w++)
//         {
//             // Decompose into fractional and integer part the coordinate
//             // coresponsing to h and w on the source matrix.
//             float new_h_frac, new_h_int, new_w_frac, new_w_int;
//             new_h_frac = modff(factor * (float)h, &new_h_int);
//             new_w_frac = modff(factor * (float)w, &new_w_int);

//             // Compute the 4 adjacent indices.
//             size_t tl_h = h0 + (size_t)new_h_int;
//             size_t tl_w = w0 + (size_t)new_w_int;
//             size_t br_h = (tl_h + 1 < m->height) ? tl_h + 1 : tl_h;
//             size_t br_w = (tl_w + 1 < m->width) ? tl_w + 1 : tl_w;

//             // Compute the 4 weights.
//             float tl_weight = (1.0f - new_h_frac) * (1.0f - new_w_frac);
//             float tr_weight = (1.0f - new_h_frac) * new_w_frac;
//             float bl_weight = new_h_frac * (1.0f - new_w_frac);
//             float br_weight = new_h_frac * new_w_frac;

//             float pixel = tl_weight * mat_coef(m, tl_h, tl_w) +
//                           tr_weight * mat_coef(m, tl_h, br_w) +
//                           bl_weight * mat_coef(m, br_h, tl_w) +
//                           br_weight * mat_coef(m, br_h, br_w);
//             *mat_unsafe_coef_ptr(res, h, w) = roundf(pixel);
//         }
//     }

//     return res;
// }

// Matrix *mat_cross_entropy(Matrix *actual, Matrix*expected)
// {
//     if (actual->height != expected->height)
//         errx(EXIT_FAILURE,
//              "Matrix hadamard product failed: mismatched heights (%zu vs
//              %zu).", actual->height, expected->height);
//     if (actual->width != expected->width)
//         errx(EXIT_FAILURE,
//              "Matrix hadamard product failed: mismatched widths (%zu vs
//              %zu).", actual->width, expected->width);

//     Matrix *res = alloc_matrix(actual->height, actual->width);

//     for (size_t i = 0; i < actual->height * actual->width, i++)
//     {
//         res->content =
//     }
// }

// double mat_mean_squared_error(Matrix *actual, Matrix *expected)
// {
//     if (actual->height != expected->height)
//         errx(EXIT_FAILURE,
//              "Matrix mean squared error calculation failed: mismatched
//              heights "
//              "(%zu vs %zu).",
//              actual->height, expected->height);
//     if (actual->width != expected->width)
//         errx(EXIT_FAILURE,
//              "Matrix mean squared error calculation failed: mismatched widths
//              "
//              "(%zu vs %zu).",
//              actual->height, expected->height);

//     double sum = 0.0;

//     for (size_t i = 0; i < actual->height * actual->width; i++)
//     {
//         double error = (actual->content[i] - expected->content[i]);
//         sum += error * error;
//     }

//     return sum / (actual->height * actual->width);
// }

Matrix *mat_transpose(const Matrix *m)
{
    Matrix *res = mat_create_zero(m->width, m->height);

    for (size_t h = 0; h < m->height; h++)
    {
        for (size_t w = 0; w < m->width; w++)
        {
            *mat_unsafe_coef_ptr(res, w, h) = *mat_unsafe_coef_ptr(m, h, w);
        }
    }

    return res;
}

void mat_inplace_transpose(Matrix *m)
{
    if (m->height == m->width)
    {
        for (size_t h = 0; h < m->height; h++)
        {
            for (size_t w = h + 1; w < m->width; w++)
            {
                size_t i = h * m->width + w;
                size_t j = w * m->width + h;
                float tmp = m->content[i];
                m->content[i] = m->content[j];
                m->content[j] = tmp;
            }
        }
    }
    else
    {
        size_t cycles = gcd(m->height, m->width);

        for (size_t i = 0; i < cycles; i++)
        {
            size_t current = i;
            float tmp = m->content[i];

            while (1)
            {
                size_t r = current / m->width;
                size_t c = current % m->width;
                size_t next = c * m->height + r;

                if (next == i)
                {
                    m->content[current] = tmp;
                    break;
                }

                m->content[current] = m->content[next];
                current = next;
            }
        }

        size_t tmp = m->height;
        m->height = m->width;
        m->width = tmp;
    }
}

Matrix *mat_vertical_flatten(const Matrix *m)
{
    Matrix *res = mat_create_zero(1, m->height * m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        res->content[i] = m->content[i];
    }

    return res;
}

void mat_inplace_vertical_flatten(Matrix *m)
{
    m->height *= m->width;
    m->width = 1;
}

Matrix *mat_horizontal_flatten(const Matrix *m)
{
    Matrix *res = mat_create_zero(m->height * m->width, 1);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        res->content[i] = m->content[i];
    }

    return res;
}

void mat_inplace_horizontal_flatten(Matrix *m)
{
    m->width *= m->height;
    m->height = 1;
}

Matrix *mat_normalize(const Matrix *m)
{
    Matrix *res = mat_create_zero(m->height, m->width);

    float sum = 0.0f;

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        sum += m->content[i];
    }

    if (sum == 0.0)
        errx(EXIT_FAILURE, "Cannot normalize a zero matrix.");

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        res->content[i] = m->content[i] / sum;
    }

    return res;
}

void mat_inplace_normalize(Matrix *m)
{
    float sum = 0.0f;

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        sum += m->content[i];
    }

    if (sum == 0.0)
        errx(EXIT_FAILURE, "Cannot normalize a zero matrix.");

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        m->content[i] /= sum;
    }
}

Matrix *mat_map(const Matrix *m, float (*f)(float))
{
    Matrix *res = mat_create_zero(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; i++)
    {
        res->content[i] = f(m->content[i]);
    }

    return res;
}

void mat_inplace_map(Matrix *m, float (*f)(float))
{
    for (size_t i = 0; i < m->height * m->width; i++)
    {
        m->content[i] = f(m->content[i]);
    }
}

Matrix *mat_map_with_indexes(const Matrix *m, float (*f)(float, size_t, size_t))
{
    Matrix *res = mat_create_zero(m->height, m->width);

    for (size_t h = 0; h < m->height; h++)
    {
        for (size_t w = 0; w < m->width; w++)
            *mat_unsafe_coef_ptr(res, h, w) =
                f(*mat_unsafe_coef_ptr(m, h, w), h, w);
    }

    return res;
}

void mat_inplace_map_with_indexes(Matrix *m, float (*f)(float, size_t, size_t))
{
    for (size_t h = 0; h < m->height; h++)
    {
        for (size_t w = 0; w < m->width; w++)
            *mat_unsafe_coef_ptr(m, h, w) =
                f(*mat_unsafe_coef_ptr(m, h, w), h, w);
    }
}

void mat_print(const Matrix *m, unsigned int precision)
{
    if (m == NULL)
    {
        errx(EXIT_FAILURE, "Given matrix pointer is null.");
    }
    else
    {
        char fmt[16];
        snprintf(fmt, sizeof(fmt), "%%.%uf", precision);

        for (size_t h = 0; h < m->height; h++)
        {
            for (size_t w = 0; w < m->width - 1; w++)
            {
                printf(fmt, mat_coef(m, h, w));
                printf("  ");
            }
            printf(fmt, mat_coef(m, h, m->width - 1));
            printf("\n");
        }
    }
}

Matrix *mat_load_from_file(char *filename)
{
    FILE *file_stream = fopen(filename, "r");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file: %s", filename);

    int fd = fileno(file_stream);
    if (fd == -1)
        errx(EXIT_FAILURE, "Failed to open file descriptor of file %s.",
             filename);

    int r_out;
    size_t height, width;

    r_out = read(fd, &height, sizeof(size_t));
    if (r_out != sizeof(size_t))
        errx(EXIT_FAILURE, "Invalid file %s: failed to read matrix's height.",
             filename);

    r_out = read(fd, &width, sizeof(size_t));
    if (r_out != sizeof(size_t))
        errx(EXIT_FAILURE, "Invalid file %s: failed to read matrix's width.",
             filename);

    Matrix *res = mat_create_zero(height, width);

    // Read the matrix content.
    for (size_t i = 0; i < height * width; i++)
    {
        r_out = read(fd, &res->content[i], sizeof(double));
        if (r_out != sizeof(double))
            errx(EXIT_FAILURE,
                 "Invalid file %s: failed to read matrix's "
                 "%zuth coefficient.",
                 filename, i);
    }

    fclose(file_stream);

    return res;
}

void mat_save_to_file(Matrix *m, char *filename)
{
    FILE *file_stream = fopen(filename, "w");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file: %s", filename);

    int fd = fileno(file_stream);
    if (fd == -1)
        errx(EXIT_FAILURE, "Failed to open file descriptor of file %s.",
             filename);

    int w_out;

    w_out = write(fd, &m->height, sizeof(size_t));
    if (w_out != sizeof(size_t))
        errx(EXIT_FAILURE,
             "Failed to write file %s: failed to write matrix's height.",
             filename);

    w_out = write(fd, &m->width, sizeof(size_t));
    if (w_out != sizeof(size_t))
        errx(EXIT_FAILURE,
             "Failed to write file %s: failed to write matrix's width.",
             filename);

    // Write the matrix content.
    for (size_t h = 0; h < m->height; h++)
    {
        for (size_t w = 0; w < m->width; w++)
        {
            w_out = write(fd, mat_unsafe_coef_ptr(m, h, w), sizeof(double));
            if (w_out != sizeof(double))
                errx(EXIT_FAILURE,
                     "Failed to write file %s: failed to write matrix's "
                     "coefficient at position (h:%zu, w:%zu).",
                     filename, h, w);
        }
    }

    fclose(file_stream);

    printf("written\n");
}

size_t mat_max_h(Matrix *m)
{
    size_t max_h = 0;
    for (size_t h = 1; h < m->height; h++)
        if (*mat_unsafe_coef_ptr(m, h, 0) > *mat_unsafe_coef_ptr(m, max_h, 0))
            max_h = h;

    return max_h;
}
