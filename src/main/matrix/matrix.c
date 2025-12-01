#include <err.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "matrix.h"
#include "utils/math/clamp.h"
#include "utils/math/gcd.h"
#include "utils/math/sigmoid.h"
#include "utils/random/random.h"

// SIMD macro handling.
#if !defined(USE_AVX)
#if defined(USE_AVX_512)
#error "USE_AVX_512 was defined but USE_AVX was not."
#elif defined(USE_AVX_2)
#error "USE_AVX_2 was defined but USE_AVX was not."
#endif
#else
#if !defined(USE_AVX_512) && !defined(USE_AVX_2)
#error "USE_AVX was defined but neither USE_AVX_2 nor USE_AVX_512 was defined."
#endif
// Define AVX macros
#include <immintrin.h>
#if defined(USE_AVX_512)
#define avx_vect_t __m512
#define avx(op, ...) _mm512_##op##_ps(__VA_ARGS__)
#define avx_vect_len 16
#elif defined(USE_AVX_2)
#define avx_vect_t __m256
#define avx(op, ...) _mm256_##op##_ps(__VA_ARGS__)
#define avx_vect_len 8
#endif
#define avx_for(counter, length)                                               \
    for (; counter <= length - avx_vect_len; counter += avx_vect_len)
#endif

/// @brief A 2D matrix of single-precision floating point numbers.
struct Matrix
{
    /// @brief Number of rows (height) of the matrix.
    size_t height;
    /// @brief Number of columns (width) of the matrix.
    size_t width;
    size_t size;
    /// @brief The matrix elements stored in a contiguous row-major array.
    float *content;
};

static inline float *alloc_content(size_t length)
{
    float *content = malloc(length * sizeof(float));

    if (content == NULL)
        errx(EXIT_FAILURE, "Memory allocation failed in alloc_content.");

    return content;
}

static inline Matrix *alloc_matrix(size_t height, size_t width)
{
    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        errx(EXIT_FAILURE, "Memory allocation failed in alloc_matrix.");

    m->height = height;
    m->width = width;
    m->size = height * width;
    m->content = alloc_content(m->size);

    return m;
}

inline size_t mat_height(const Matrix *m) { return m->height; }

inline size_t mat_width(const Matrix *m) { return m->width; }

Matrix *mat_create(size_t height, size_t width)
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

    return alloc_matrix(height, width);
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

    Matrix *m = alloc_matrix(height, width);
    memset(m->content, 0, m->size * sizeof(float));

    return m;
}

Matrix *mat_create_filled(size_t height, size_t width, float value)
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

    Matrix *m = alloc_matrix(height, width);

    float *c = m->content;

#if defined(USE_AVX)
    avx_vect_t v = avx(set1, value);

    size_t i = 0;
    avx_for(i, m->size) avx(storeu, &c[i], v);

    for (; i < m->size; ++i)
        c[i] = value;
#else
    for (size_t i = 0; i < m->size; ++i)
        c[i] = value;
#endif

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

    Matrix *m = alloc_matrix(height, width);

    memcpy(m->content, content, m->size * sizeof(float));

    return m;
}

Matrix *mat_create_from_2d_arr(size_t height, size_t width,
                               const float **content)
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

    Matrix *m = alloc_matrix(height, width);

    for (size_t h = 0; h < height; ++h)
        memcpy(&m->content[h * width], content[h], width * sizeof(float));

    return m;
}

Matrix *mat_create_random_uniform(size_t height, size_t width, float min,
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

    Matrix *m = alloc_matrix(height, width);

    float *c = m->content;
    for (size_t i = 0; i < m->size; ++i)
        c[i] = rand_f_uniform_nm(min, max);

    return m;
}

Matrix *mat_create_random_gaussian(size_t height, size_t width)
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

    Matrix *m = alloc_matrix(height, width);

    float *c = m->content;
    for (size_t i = 0; i < m->size; ++i)
        c[i] = rand_f_gaussian();

    return m;
}

Matrix *mat_create_random_normal(size_t height, size_t width, float mean,
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

    Matrix *m = alloc_matrix(height, width);

    float *c = m->content;
    for (size_t i = 0; i < m->size; ++i)
        c[i] = rand_f_normal(mean, stddev);

    return m;
}

void mat_free(Matrix *matrix)
{
#ifdef USE_AVX
    _mm_free(matrix->content);
#else
    free(matrix->content);
#endif
    free(matrix);
}

void mat_free_matrix_array(Matrix **array, size_t lentgh)
{
    for (size_t i = 0; i < lentgh; ++i)
    {
        if (array[i] != NULL)
            mat_free(array[i]);
    }
    free(array);
}

int mat_eq(Matrix *a, Matrix *b, float epsilon)
{
    if (a == b)
        return 1;

    if (a->height != b->height || a->width != b->width)
        return 0;

    for (size_t i = 0; i < a->height * a->width; ++i)
        if (fabsf(a->content[i] - b->content[i]) > epsilon)
            return 0;

    return 1;
}

Matrix *mat_deepcopy(const Matrix *src)
{
    Matrix *dst = alloc_matrix(src->height, src->width);
    memcpy(dst->content, src->content,
           src->height * src->width * sizeof(float));
    return dst;
}

inline float *mat_unsafe_coef_ptr(const Matrix *m, size_t h, size_t w)
{
    return m->content + h * m->width + w;
}

inline float *mat_coef_ptr(const Matrix *m, size_t h, size_t w)
{
    if (h >= m->height)
        errx(EXIT_FAILURE, "Invalid height given. Expected < %zu and got %zu.",
             m->height, h);
    if (w >= m->width)
        errx(EXIT_FAILURE, "Invalid width given. Expected < %zu and got %zu.",
             m->width, w);
    return mat_unsafe_coef_ptr(m, h, w);
}

inline float mat_coef(const Matrix *m, size_t h, size_t w)
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

    Matrix *res = alloc_matrix(a->height, b->width);
#ifdef USE_AVX
    size_t i = 0;
    avx_for(i, res->size)
    {
        avx_vect_t a_v = avx(loadu, &a->content[i]);
        avx_vect_t b_v = avx(loadu, &b->content[i]);
        avx_vect_t c_v = avx(add, a_v, b_v);
        avx(storeu, &res->content[i], c_v);
    }
    for (; i < res->height * res->width; ++i)
        res->content[i] = a->content[i] + b->content[i];
#else
    for (size_t i = 0; i < res->height * res->width; ++i)
        res->content[i] = a->content[i] + b->content[i];
#endif
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

#ifdef USE_AVX
    size_t i = 0;
    avx_for(i, a->size)
    {
        avx_vect_t a_v = avx(loadu, &a->content[i]);
        avx_vect_t b_v = avx(loadu, &b->content[i]);
        avx_vect_t c_v = avx(add, a_v, b_v);
        avx(storeu, &a->content[i], c_v);
    }
    for (; i < a->height * a->width; ++i)
        a->content[i] += b->content[i];
#else
    for (size_t i = 0; i < a->height * a->width; ++i)
        a->content[i] += b->content[i];
#endif
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

    Matrix *res = alloc_matrix(a->height, b->width);
#ifdef USE_AVX
    size_t i = 0;
    avx_for(i, res->size)
    {
        avx_vect_t a_v = avx(loadu, &a->content[i]);
        avx_vect_t b_v = avx(loadu, &b->content[i]);
        avx_vect_t c_v = avx(sub, a_v, b_v);
        avx(storeu, &res->content[i], c_v);
    }
    for (; i < res->height * res->width; ++i)
        res->content[i] = a->content[i] - b->content[i];
#else
    for (size_t i = 0; i < res->height * res->width; ++i)
        res->content[i] = a->content[i] - b->content[i];
#endif
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

#ifdef USE_AVX
    size_t i = 0;
    avx_for(i, a->size)
    {
        avx_vect_t a_v = avx(loadu, &a->content[i]);
        avx_vect_t b_v = avx(loadu, &b->content[i]);
        avx_vect_t c_v = avx(sub, a_v, b_v);
        avx(storeu, &a->content[i], c_v);
    }
    for (; i < a->height * a->width; ++i)
        a->content[i] -= b->content[i];
#else
    for (size_t i = 0; i < a->height * a->width; ++i)
        a->content[i] -= b->content[i];
#endif
}

Matrix *mat_scalar_multiplication(const Matrix *m, float a)
{
    Matrix *res = alloc_matrix(m->height, m->width);

#ifdef USE_AVX
    avx_vect_t a_v = avx(set1, a);
    size_t i = 0;
    avx_for(i, m->size)
    {
        avx_vect_t m_v = avx(loadu, &m->content[i]);
        avx_vect_t r_v = avx(mul, a_v, m_v);
        avx(storeu, &res->content[i], r_v);
    }
    for (; i < m->height * m->width; ++i)
        res->content[i] = a * m->content[i];
#else
    for (size_t i = 0; i < m->height * m->width; ++i)
        res->content[i] = a * m->content[i];
#endif

    return res;
}

void mat_inplace_scalar_multiplication(Matrix *m, float a)
{
#ifdef USE_AVX
    avx_vect_t a_v = avx(set1, a);
    size_t i = 0;
    avx_for(i, m->size)
    {
        avx_vect_t m_v = avx(loadu, &m->content[i]);
        avx_vect_t r_v = avx(mul, a_v, m_v);
        avx(storeu, &m->content[i], r_v);
    }
    for (; i < m->height * m->width; ++i)
        m->content[i] *= a;
#else
    for (size_t i = 0; i < m->height * m->width; ++i)
        m->content[i] *= a;
#endif
}

Matrix *mat_multiplication(const Matrix *a, const Matrix *b)
{
    if (a->width != b->height)
        errx(EXIT_FAILURE,
             "Cannot multiply two matrices if the width of the first does "
             "not match the height of the second.");

    Matrix *res = alloc_matrix(a->height, b->width);
#ifdef USE_AVX
    size_t middle_dim = a->width; // or b->height

    // The transpose matrix of b.
    Matrix *b_t = mat_transpose(b);

    for (size_t h = 0; h < res->height; ++h)
    {
        for (size_t w = 0; w < res->width; ++w)
        {
            // or &a->content[h * middle_dim]
            const float *a_row = mat_unsafe_coef_ptr(a, h, 0);
            // or &b_t->content[w * middle_dim]
            const float *b_col = mat_unsafe_coef_ptr(b_t, w, 0);

            avx_vect_t sum = avx(setzero);

            size_t k = 0;
            avx_for(k, middle_dim)
            {
                avx_vect_t a_v = avx(loadu, &a_row[k]);
                avx_vect_t b_v = avx(loadu, &b_col[k]);
                sum = avx(fmadd, a_v, b_v, sum);
            }
#ifdef AVX512
            float tmp[16];
            avx(storeu, tmp, sum);
            float total = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] +
                          tmp[6] + tmp[7] + tmp[8] + tmp[9] + tmp[10] +
                          tmp[11] + tmp[12] + tmp[13] + tmp[14] + tmp[15];
#else
            float tmp[8];
            avx(storeu, tmp, sum);
            float total = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] +
                          tmp[6] + tmp[7];
#endif
            for (; k < middle_dim; ++k)
                total += a_row[k] * b_col[k];

            res->content[h * res->width + w] = total;
        }
    }

    mat_free(b_t);
#else
    for (size_t h = 0; h < res->height; ++h)
    {
        for (size_t w = 0; w < res->width; ++w)
        {
            float sum = 0.0f;
            for (size_t k = 0; k < a->width; ++k)
            {
                sum += *mat_unsafe_coef_ptr(a, h, k) *
                       *mat_unsafe_coef_ptr(b, k, w);
            }
            *mat_unsafe_coef_ptr(res, h, w) = sum;
        }
    }
#endif
    return res;
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

    Matrix *res = alloc_matrix(a->height, a->width);
#ifdef USE_AVX
    size_t i = 0;
    avx_for(i, res->size)
    {
        avx_vect_t a_v = avx(loadu, &a->content[i]);
        avx_vect_t b_v = avx(loadu, &b->content[i]);
        avx_vect_t r_v = avx(mul, a_v, b_v);
        avx(storeu, &res->content[i], r_v);
    }
    for (; i < res->height * res->width; ++i)
        res->content[i] = a->content[i] * b->content[i];
#else
    for (size_t i = 0; i < a->height * a->width; ++i)
        res->content[i] = a->content[i] * b->content[i];
#endif
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

#ifdef USE_AVX
    size_t i = 0;
    avx_for(i, a->size)
    {
        avx_vect_t a_v = avx(loadu, &a->content[i]);
        avx_vect_t b_v = avx(loadu, &b->content[i]);
        avx_vect_t r_v = avx(mul, a_v, b_v);
        avx(storeu, &a->content[i], r_v);
    }
    for (; i < a->height * a->width; ++i)
        a->content[i] *= b->content[i];
#else
    for (size_t i = 0; i < a->height * a->width; ++i)
        a->content[i] *= b->content[i];
#endif
}

Matrix *mat_relu(Matrix *m)
{
    Matrix *res = alloc_matrix(m->height, m->width);

    float *m_c = m->content;
    float *r_c = res->content;
#ifdef USE_AVX
    avx_vect_t zero = avx(setzero);

    size_t i = 0;
    avx_for(i, m->size)
    {
        avx_vect_t a_v = avx(loadu, &m_c[i]);
        avx_vect_t r_v = avx(max, a_v, zero);
        avx(storeu, &r_c[i], r_v);
    }
    for (; i < m->height * m->width; ++i)
        r_c[i] = m_c[i] > 0.0f ? m_c[i] : 0.0f;
#else
    for (size_t i = 0; i < m->height * m->width; ++i)
        r_c[i] = m_c[i] > 0.0f ? m_c[i] : 0.0f;
#endif
    return res;
}

void mat_inplace_relu(Matrix *m)
{
    float *m_c = m->content;
#ifdef USE_AVX
    avx_vect_t zero = avx(setzero);

    size_t i = 0;
    avx_for(i, m->size)
    {
        avx_vect_t a_v = avx(loadu, &m_c[i]);
        avx_vect_t r_v = avx(max, a_v, zero);
        avx(storeu, &m_c[i], r_v);
    }
    for (; i < m->height * m->width; ++i)
        m_c[i] = m_c[i] > 0.0f ? m_c[i] : 0.0f;
#else
    for (size_t i = 0; i < m->height * m->width; ++i)
        m_c[i] = m_c[i] > 0.0f ? m_c[i] : 0.0f;
#endif
}

Matrix *mat_relu_derivative(Matrix *m)
{
    Matrix *res = mat_create_zero(m->height, m->width);

    float *src = m->content;
    float *dst = res->content;
#ifdef USE_AVX
    avx_vect_t zero = avx(setzero);
    avx_vect_t one = avx(set1, 1.0f);

    size_t i = 0;
    avx_for(i, m->size)
    {
        avx_vect_t v = avx(loadu, src + i);
        avx_vect_t mask = avx(cmp, v, zero, _CMP_GT_OQ);
        avx_vect_t r = avx(and, mask, one);
        avx(storeu, dst + i, r);
    }
    for (; i < m->height * m->width; ++i)
        dst[i] = src[i] > 0.0f ? 1.0f : 0.0f;
#else
    for (size_t i = 0; i < m->height * m->width; ++i)
        dst[i] = src[i] > 0.0f ? 1.0f : 0.0f;
#endif
    return res;
}

void mat_inplace_relu_derivative(Matrix *m)
{
    float *c = m->content;
#ifdef USE_AVX
    avx_vect_t zero = avx(setzero);
    avx_vect_t one = avx(set1, 1.0f);

    size_t i = 0;
    avx_for(i, m->size)
    {
        avx_vect_t v = avx(loadu, c + i);
        avx_vect_t mask = avx(cmp, v, zero, _CMP_GT_OQ);
        avx_vect_t r = avx(and, mask, one);
        avx(storeu, c + i, r);
    }
    for (; i < m->height * m->width; ++i)
        c[i] = c[i] > 0.0f ? 1.0f : 0.0f;
#else
    for (size_t i = 0; i < m->height * m->width; ++i)
        c[i] = c[i] > 0.0f ? 1.0f : 0.0f;
#endif
}

// HERE

#if defined(USE_AVX_512)
// static avx_vect_t _mm256_exp_ps(){}
#error "AVX_512 is not supported yet."
#elif defined(USE_AVX_2)
static avx_vect_t _mm256_exp_ps(avx_vect_t x)
{
    const avx_vect_t ln2 = avx(set1, 0.69314718056f);
    const avx_vect_t inv_ln2 = avx(set1, 1.44269504089f);

    // Range reduction: x = n*ln2 + r, where r in [-ln2/2, ln2/2]
    avx_vect_t n = avx(mul, x, inv_ln2);
    n = avx(round, n, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

    avx_vect_t y = avx(mul, n, ln2);
    avx_vect_t r = avx(sub, x, y);

    // Polynomial approximation of exp(r)
    const avx_vect_t c1 = avx(set1, 1.0f);
    const avx_vect_t c2 = avx(set1, 0.4999999403953552f);
    const avx_vect_t c3 = avx(set1, 0.16666594183444977f);
    const avx_vect_t c4 = avx(set1, 0.041657347708940506f);
    const avx_vect_t c5 = avx(set1, 0.008301359802305698f);
    const avx_vect_t c6 = avx(set1, 0.0013298822781072855f);

    avx_vect_t r2 = avx(mul, r, r);
    avx_vect_t poly = c6;
    poly = avx(fmadd, poly, r, c5);
    poly = avx(fmadd, poly, r, c4);
    poly = avx(fmadd, poly, r, c3);
    poly = avx(fmadd, poly, r, c2);
    poly = avx(fmadd, poly, r2, r);
    poly = avx(add, poly, c1);

    // Reconstruct exp(x) = exp(r) * 2^n
    __m256i pow2n = _mm256_slli_epi32(
        _mm256_add_epi32(_mm256_cvtps_epi32(n), _mm256_set1_epi32(127)), 23);
    avx_vect_t pow2 = avx(castsi256, pow2n);

    return avx(mul, poly, pow2);
}
#endif

void mat_inplace_softmax(Matrix *m)
{
    size_t n = m->height * m->width;

    float max_val = m->content[0];
    for (size_t i = 1; i < n; ++i)
        if (m->content[i] > max_val)
            max_val = m->content[i];

    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i)
    {
        m->content[i] = expf(m->content[i] - max_val);
        sum += m->content[i];
    }

    for (size_t i = 0; i < n; ++i)
        m->content[i] /= sum;
}

void mat_inplace_toggle(Matrix *m)
{
    for (size_t i = 0; i < m->size; ++i)
        m->content[i] = m->content[i] < 0.5f ? 1.0f : 0.0f;
}

Matrix *mat_strip_margins(const Matrix *m)
{
    // Whether an activated (<=> black <=> 1.0f) pixel has been found.
    int found;
    // The new coordinates (included).
    size_t h_i = 0, h_f = m->height - 1, w_i = 0, w_f = m->width - 1;

    found = 0;
    while (h_i < m->height && !found)
    {
        for (size_t w = 0; w < m->width && !found; ++w)
            found = *mat_coef_ptr(m, h_i, w) > 0.5f;
        h_i++;
    }
    h_i--;

    found = 0;
    while (h_f > 0 && !found)
    {
        for (size_t w = 0; w < m->width && !found; ++w)
            found = *mat_coef_ptr(m, h_f, w) > 0.5f;
        h_f--;
    }
    h_f++;

    found = 0;
    while (w_i < m->width && !found)
    {
        for (size_t h = 0; h < m->height && !found; ++h)
            found = *mat_coef_ptr(m, h, w_i) > 0.5f;
        w_i++;
    }
    w_i--;

    found = 0;
    while (w_f > 0 && !found)
    {
        for (size_t h = 0; h < m->height && !found; ++h)
            found = *mat_coef_ptr(m, h, w_f) > 0.5f;
        w_f--;
    }
    w_f++;

    if (h_i > h_f || w_i > w_f)
        return NULL;

    Matrix *res = mat_create_zero(h_f - h_i + 1, w_f - w_i + 1);
    for (size_t h = 0; h < res->height; ++h)
        for (size_t w = 0; w < res->width; ++w)
            *mat_coef_ptr(res, h, w) = *mat_coef_ptr(m, h_i + h, w_i + w);

    return res;
}

Matrix *mat_scale_to_28(const Matrix *m, float fill_value)
{
    const size_t dim = 28;
    Matrix *res = mat_create_filled(dim, dim, fill_value);

    // Compute scaling factors for height and width
    float scale_h = (float)m->height / (float)dim;
    float scale_w = (float)m->width / (float)dim;

    // Use the larger scale to preserve aspect ratio
    float factor = (scale_h > scale_w) ? scale_h : scale_w;

    // Compute offset to center the content
    float h_offset = ((dim * factor) - m->height) / 2.0f;
    float w_offset = ((dim * factor) - m->width) / 2.0f;

    for (size_t h = 0; h < dim; ++h)
    {
        for (size_t w = 0; w < dim; ++w)
        {
            // Map target pixel to source matrix coordinates
            float sh = h * factor - h_offset;
            float sw = w * factor - w_offset;

            // Clamp to valid source range
            if (sh < 0 || sw < 0 || sh > m->height - 1 || sw > m->width - 1)
                continue;
            //     sh = 0;
            // if (sw < 0)
            //     sw = 0;
            // if (sh > m->height - 1)
            //     sh = m->height - 1;
            // if (sw > m->width - 1)
            //     sw = m->width - 1;

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

void mat_inplace_to_one_hot(Matrix *m)
{
    for (size_t i = 0; i < m->height * m->width; ++i)
        m->content[i] = m->content[i] > 0.5f ? 1.0f : 0.0f;
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

// float mat_mean_squared_error(Matrix *actual, Matrix *expected)
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

//     float sum = 0.0f;

//     for (size_t i = 0; i < actual->height * actual->width; i++)
//     {
//         float error = (actual->content[i] - expected->content[i]);
//         sum += error * error;
//     }

//     return sum / (actual->height * actual->width);
// }

#ifdef USE_AVX
#if 0 // def USE_AVX_512
#else
static inline void transpose_block(const float *src, float *dst,
                                   size_t src_width, size_t dst_width)
{
    avx_vect_t row0 = avx(loadu, src + 0 * src_width);
    avx_vect_t row1 = avx(loadu, src + 1 * src_width);
    avx_vect_t row2 = avx(loadu, src + 2 * src_width);
    avx_vect_t row3 = avx(loadu, src + 3 * src_width);
    avx_vect_t row4 = avx(loadu, src + 4 * src_width);
    avx_vect_t row5 = avx(loadu, src + 5 * src_width);
    avx_vect_t row6 = avx(loadu, src + 6 * src_width);
    avx_vect_t row7 = avx(loadu, src + 7 * src_width);

    // Step 1: unpack pairs
    avx_vect_t t0 = avx(unpacklo, row0, row1);
    avx_vect_t t1 = avx(unpackhi, row0, row1);
    avx_vect_t t2 = avx(unpacklo, row2, row3);
    avx_vect_t t3 = avx(unpackhi, row2, row3);
    avx_vect_t t4 = avx(unpacklo, row4, row5);
    avx_vect_t t5 = avx(unpackhi, row4, row5);
    avx_vect_t t6 = avx(unpacklo, row6, row7);
    avx_vect_t t7 = avx(unpackhi, row6, row7);

    // Step 2: shuffle 128-bit lanes
    avx_vect_t s0 = avx(shuffle, t0, t2, 0x4E);
    avx_vect_t s1 = avx(shuffle, t1, t3, 0x4E);
    avx_vect_t s2 = avx(shuffle, t4, t6, 0x4E);
    avx_vect_t s3 = avx(shuffle, t5, t7, 0x4E);

    // Step 3: final permutation
    avx_vect_t r0 = avx(permute2f128, s0, s2, 0x20);
    avx_vect_t r1 = avx(permute2f128, s1, s3, 0x20);
    avx_vect_t r2 = avx(permute2f128, s0, s2, 0x31);
    avx_vect_t r3 = avx(permute2f128, s1, s3, 0x31);

    // Final interleaving step
    avx_vect_t r4 = avx(permute2f128, s0, s2, 0x02);
    avx_vect_t r5 = avx(permute2f128, s1, s3, 0x02);
    avx_vect_t r6 = avx(permute2f128, s0, s2, 0x13);
    avx_vect_t r7 = avx(permute2f128, s1, s3, 0x13);

    // Store result (column-major into output row-major)
    avx(storeu, dst + 0 * dst_width, r0);
    avx(storeu, dst + 1 * dst_width, r1);
    avx(storeu, dst + 2 * dst_width, r2);
    avx(storeu, dst + 3 * dst_width, r3);
    avx(storeu, dst + 4 * dst_width, r4);
    avx(storeu, dst + 5 * dst_width, r5);
    avx(storeu, dst + 6 * dst_width, r6);
    avx(storeu, dst + 7 * dst_width, r7);
}
#endif
#endif

// void mat_transpose(const Matrix *m)
// {
//     Matrix *res = alloc_matrix(m->width, m->height);

//     const float *src = m->content;
//     float *B = m->content;

//     const size_t block = 8;

//     size_t N8 = N - (N % block);
//     size_t M8 = M - (M % block);

//     // Process full 8x8 blocks
//     for (size_t i = 0; i < N8; i += block)
//     {
//         for (size_t j = 0; j < M8; j += block)
//         {
//             transpose_8x8_block(&A[i * M + j], &B[j * N + i],
//                                 M, // src row width
//                                 N  // dst row width
//             );
//         }
//     }

//     // Remainder cols (scalar)
//     for (size_t i = 0; i < N; ++i)
//     {
//         for (size_t j = M8; j < M; ++j)
//         {
//             B[j * N + i] = A[i * M + j];
//         }
//     }

//     // Remainder rows (scalar)
//     for (size_t i = N8; i < N; ++i)
//     {
//         for (size_t j = 0; j < M8; ++j)
//         {
//             B[j * N + i] = A[i * M + j];
//         }
//     }
// }

Matrix *mat_transpose(const Matrix *m)
{
    Matrix *res = alloc_matrix(m->width, m->height);

#if 0
    // Handle blocks.
    for (size_t h = 0; h < m->height; h += 4)
        for (size_t w = 0; w < m->width; w += 4)
            transpose_block(m, res, h, w);

    // Handle remaining rows.
    for (size_t h = m->height - m->height % 4; h < m->height; ++h)
        for (size_t w = 0; w < m->width; ++w)
            *mat_unsafe_coef_ptr(res, w, h) = *mat_unsafe_coef_ptr(m, h, w);

    // Handle remaining columns.
    for (size_t h = 0; h < m->height - m->height % 4; ++h)
        for (size_t w = m->width - m->width % 4; w < m->width; ++w)
            *mat_unsafe_coef_ptr(res, w, h) = *mat_unsafe_coef_ptr(m, h, w);

#else
    for (size_t h = 0; h < m->height; ++h)
    {
        for (size_t w = 0; w < m->width; ++w)
        {
            *mat_unsafe_coef_ptr(res, w, h) = *mat_unsafe_coef_ptr(m, h, w);
        }
    }
#endif

    return res;
}

// void mat_inplace_transpose(Matrix *m)
// {
//     if (m->height == m->width)
//     {
//         for (size_t h = 0; h < m->height; h++)
//         {
//             for (size_t w = h + 1; w < m->width; w++)
//             {
//                 size_t i = h * m->width + w;
//                 size_t j = w * m->width + h;
//                 float tmp = m->content[i];
//                 m->content[i] = m->content[j];
//                 m->content[j] = tmp;
//             }
//         }
//     }
//     else
//     {
//         size_t cycles = gcd(m->height, m->width);

//         for (size_t i = 0; i < cycles; i++)
//         {
//             size_t current = i;
//             float tmp = m->content[i];

//             while (1)
//             {
//                 size_t r = current / m->width;
//                 size_t c = current % m->width;
//                 size_t next = c * m->height + r;

//                 if (next == i)
//                 {
//                     m->content[current] = tmp;
//                     break;
//                 }

//                 m->content[current] = m->content[next];
//                 current = next;
//             }
//         }

//         size_t tmp = m->height;
//         m->height = m->width;
//         m->width = tmp;
//     }
// }

// Matrix *mat_vertical_flatten(const Matrix *m)
// {
//     Matrix *res = mat_create_zero(1, m->height * m->width);

//     for (size_t i = 0; i < m->height * m->width; i++)
//     {
//         res->content[i] = m->content[i];
//     }

//     return res;
// }

void mat_inplace_vertical_flatten(Matrix *m)
{
    m->height *= m->width;
    m->width = 1;
}

// Matrix *mat_horizontal_flatten(const Matrix *m)
// {
//     Matrix *res = mat_create_zero(m->height * m->width, 1);

//     for (size_t i = 0; i < m->height * m->width; i++)
//     {
//         res->content[i] = m->content[i];
//     }

//     return res;
// }

// void mat_inplace_horizontal_flatten(Matrix *m)
// {
//     m->width *= m->height;
//     m->height = 1;
// }

Matrix *mat_normalize(const Matrix *m)
{
    Matrix *res = mat_create_zero(m->height, m->width);

    float sum = 0.0f;

    for (size_t i = 0; i < m->height * m->width; ++i)
    {
        sum += m->content[i];
    }

    if (sum == 0.0)
        errx(EXIT_FAILURE, "Cannot normalize a zero matrix.");

    for (size_t i = 0; i < m->height * m->width; ++i)
    {
        res->content[i] = m->content[i] / sum;
    }

    return res;
}

void mat_inplace_normalize(Matrix *m)
{
    float sum = 0.0f;

    for (size_t i = 0; i < m->height * m->width; ++i)
    {
        sum += m->content[i];
    }

    if (sum == 0.0)
        errx(EXIT_FAILURE, "Cannot normalize a zero matrix.");

    for (size_t i = 0; i < m->height * m->width; ++i)
    {
        m->content[i] /= sum;
    }
}

Matrix *mat_map(const Matrix *m, float (*f)(float))
{
    Matrix *res = mat_create_zero(m->height, m->width);

    for (size_t i = 0; i < m->height * m->width; ++i)
    {
        res->content[i] = f(m->content[i]);
    }

    return res;
}

void mat_inplace_map(Matrix *m, float (*f)(float))
{
    for (size_t i = 0; i < m->height * m->width; ++i)
    {
        m->content[i] = f(m->content[i]);
    }
}

Matrix *mat_map_with_indexes(const Matrix *m, float (*f)(float, size_t, size_t))
{
    Matrix *res = mat_create_zero(m->height, m->width);

    for (size_t h = 0; h < m->height; ++h)
    {
        for (size_t w = 0; w < m->width; ++w)
            *mat_unsafe_coef_ptr(res, h, w) =
                f(*mat_unsafe_coef_ptr(m, h, w), h, w);
    }

    return res;
}

void mat_inplace_map_with_indexes(Matrix *m, float (*f)(float, size_t, size_t))
{
    for (size_t h = 0; h < m->height; ++h)
    {
        for (size_t w = 0; w < m->width; ++w)
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

        for (size_t h = 0; h < m->height; ++h)
        {
            for (size_t w = 0; w < m->width - 1; ++w)
            {
                printf(fmt, mat_coef(m, h, w));
                printf("  ");
            }
            printf(fmt, mat_coef(m, h, m->width - 1));
            printf("\n");
        }
    }
}

void mat_print_n_first(const Matrix *m, size_t n, unsigned int precision)
{
    if (m == NULL)
        errx(EXIT_FAILURE, "Given matrix pointer is null.");

    if (n > m->height * m->width)
        errx(EXIT_FAILURE,
             "Cannot print more coefficient that the matrix has.");

    char fmt[16];
    snprintf(fmt, sizeof(fmt), "%%.%uf", precision);

    for (size_t i = 0; i < n; ++i)
    {
        printf(fmt, m->content[i]);
        printf("  ");
    }
    printf("\n");
}

void mat_display(const Matrix *m)
{
    char top_ansi[64];
    char bot_ansi[64];

    for (size_t h = 0; h < m->height; h += 2)
    {
        for (size_t w = 0; w < m->width; ++w)
        {
            int top_grey = clamp(
                (int)(*mat_unsafe_coef_ptr(m, h, w) * 255.0f + 0.5f), 0, 255);
            snprintf(top_ansi, 64, "\x1b[38;2;%i;%i;%im", top_grey, top_grey,
                     top_grey);

            if (h >= m->height)
            {
                bot_ansi[0] = '\0';
            }
            else
            {
                int bot_grey = clamp(
                    (int)(*mat_unsafe_coef_ptr(m, h + 1, w) * 255.0f + 0.5f), 0,
                    255);
                snprintf(bot_ansi, 64, "\x1b[48;2;%i;%i;%im", bot_grey,
                         bot_grey, bot_grey);
            }

            printf("%s%s▀", top_ansi, bot_ansi);
        }

        printf("\x1b[0m\n");
    }
}

Matrix *mat_load_from_fd(int fd)
{
    int r_out;
    size_t height, width;

    r_out = read(fd, &height, sizeof(size_t));
    if (r_out != sizeof(size_t))
        errx(EXIT_FAILURE, "Invalid file: failed to read matrix's height.");

    r_out = read(fd, &width, sizeof(size_t));
    if (r_out != sizeof(size_t))
        errx(EXIT_FAILURE, "Invalid file: failed to read matrix's width.");

    Matrix *res = alloc_matrix(height, width);

    // Read the matrix content.
    for (size_t i = 0; i < res->size; ++i)
    {
        r_out = read(fd, &res->content[i], sizeof(float));
        if (r_out != sizeof(float))
            errx(EXIT_FAILURE,
                 "Invalid file: failed to read matrix's "
                 "%zuth coefficient.",
                 i);
    }

    return res;
}

Matrix *mat_load_from_file(const char *filename)
{
    FILE *file_stream = fopen(filename, "r");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file: %s", filename);

    int fd = fileno(file_stream);
    if (fd == -1)
        errx(EXIT_FAILURE, "Failed to open file descriptor of file %s.",
             filename);

    Matrix *res = mat_load_from_fd(fd);

    if (getc(file_stream) != EOF)
        printf("WARNING: matrix %s is larger than expected.", filename);

    fclose(file_stream);

    return res;
}

void mat_save_to_fd(const Matrix *m, int fd)
{
    int w_out;

    w_out = write(fd, &m->height, sizeof(size_t));
    if (w_out != sizeof(size_t))
        errx(EXIT_FAILURE,
             "Failed to write file: failed to write matrix's height.");

    w_out = write(fd, &m->width, sizeof(size_t));
    if (w_out != sizeof(size_t))
        errx(EXIT_FAILURE,
             "Failed to write file: failed to write matrix's width.");

    // Write the matrix content.
    for (size_t h = 0; h < m->height; ++h)
    {
        for (size_t w = 0; w < m->width; ++w)
        {
            w_out = write(fd, mat_unsafe_coef_ptr(m, h, w), sizeof(float));
            if (w_out != sizeof(float))
                errx(EXIT_FAILURE,
                     "Failed to write file: failed to write matrix's "
                     "coefficient at position (h:%zu, w:%zu).",
                     h, w);
        }
    }
}

void mat_save_to_file(const Matrix *m, const char *filename)
{
    FILE *file_stream = fopen(filename, "w");
    if (file_stream == NULL)
        errx(EXIT_FAILURE, "Failed to open file: %s", filename);

    int fd = fileno(file_stream);
    if (fd == -1)
        errx(EXIT_FAILURE, "Failed to open file descriptor of file %s.",
             filename);

    mat_save_to_fd(m, fd);

    fclose(file_stream);
}

size_t mat_max_h(const Matrix *m)
{
    size_t max_h = 0;
    for (size_t h = 1; h < m->height; ++h)
        if (*mat_unsafe_coef_ptr(m, h, 0) > *mat_unsafe_coef_ptr(m, max_h, 0))
            max_h = h;

    return max_h;
}
