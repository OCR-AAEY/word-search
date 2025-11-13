#include "rotation.h"
#include "pretreatment/visualization.h"
#include <err.h>
#include <math.h>
#include <stdlib.h>

Matrix *rotate_matrix(const Matrix *src, float angle)
{
    if (src == NULL)
    {
        errx(EXIT_FAILURE, "rotate_matrix: src matrix is NULL");
    }

    size_t w = mat_width(src);
    size_t h = mat_height(src);

    float rad = angle * M_PI / 180.0f;

    size_t nw =
        (size_t)(fabsf((float)w * cosf(rad)) + fabsf((float)h * sinf(rad)) + 0.5f);
    size_t nh =
        (size_t)(fabsf((float)h * cosf(rad)) + fabsf((float)w * sinf(rad)) + 0.5f);

    Matrix *rotated = mat_create_zero(nh, nw);

    for (size_t y = 0; y < nh; y++)
    {
        for (size_t x = 0; x < nw; x++)
        {
            *mat_coef_ptr(rotated, y, x) = 255.0f;
        }
    }

    float cx = (float)w / 2.0f;
    float cy = (float)h / 2.0f;
    float ncx = (float)nw / 2.0f;
    float ncy = (float)nh / 2.0f;

    for (size_t y = 0; y < nh; y++)
    {
        for (size_t x = 0; x < nw; x++)
        {
            float tx = ((float)x - ncx) * cosf(rad) - ((float)y - ncy) * sinf(rad) + cx;
            float ty = ((float)x - ncx) * sinf(rad) + ((float)y - ncy) * cosf(rad) + cy;

            if (tx >= 0.0f && tx < (float)w && ty >= 0.0f && ty < (float)h)
            {
                int ix = (int)tx;
                int iy = (int)ty;
                float val = mat_coef(src, iy, ix);
                *mat_coef_ptr(rotated, y, x) = val;
            }
        }
    }

    return rotated;
}

ImageData *rotate_image(ImageData *img, float angle)
{
    int w = img->width;
    int h = img->height;
    Pixel *p = img->pixels;

    float rad = angle * M_PI / 180.0;

    int nw = (int)(fabsf((float)w * cosf(rad)) + fabsf((float)h * sinf(rad)) + 0.5f);
    int nh = (int)(fabsf((float)h * cosf(rad)) + fabsf((float)w * sinf(rad)) + 0.5f);

    Pixel *np = calloc(nw * nh, sizeof(Pixel));
    if (!np)
    {
        errx(EXIT_FAILURE, "rotate_image malloc fail");
    }

    for (int i = 0; i < nw * nh; i++)
    {
        np[i].r = 255;
        np[i].g = 255;
        np[i].b = 255;
    }

    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float ncx = nw / 2.0f;
    float ncy = nh / 2.0f;

    for (int y = 0; y < nh; y++)
    {
        for (int x = 0; x < nw; x++)
        {
            float tx = ((float)x - ncx) * cosf(rad) - ((float)y - ncy) * sinf(rad) + cx;
            float ty = ((float)x - ncx) * sinf(rad) + ((float)y - ncy) * cosf(rad) + cy;

            if (tx >= 0 && tx < w && ty >= 0 && ty < h)
            {
                int ix = (int)tx;
                int iy = (int)ty;
                np[y * nw + x] = p[iy * w + ix];
            }
        }
    }

    ImageData *out = malloc(sizeof(ImageData));
    if (!out)
    {
        errx(EXIT_FAILURE, "rotate_image malloc fail out");
    }

    out->width = nw;
    out->height = nh;
    out->pixels = np;

    return out;
}
