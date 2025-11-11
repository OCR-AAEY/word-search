#include "rotation.h"
#include "pretreatment/visualization.h"
#include <err.h>
#include <math.h>
#include <stdlib.h>

Matrix *rotate_matrix(const Matrix *src, double angle)
{
    if (src == NULL)
    {
        errx(EXIT_FAILURE, "rotate_matrix: src matrix is NULL");
    }

    size_t w = (size_t)mat_width(src);
    size_t h = (size_t)mat_height(src);

    double rad = angle * M_PI / 180.0;

    size_t nw =
        (size_t)(fabs((double)w * cos(rad)) + fabs((double)h * sin(rad)) + 0.5);
    size_t nh =
        (size_t)(fabs((double)h * cos(rad)) + fabs((double)w * sin(rad)) + 0.5);

    Matrix *rotated = mat_create_empty(nh, nw);

    for (size_t y = 0; y < nh; y++)
    {
        for (size_t x = 0; x < nw; x++)
        {
            *mat_coef_ptr(rotated, y, x) = 255.0;
        }
    }

    double cx = w / 2.0;
    double cy = h / 2.0;
    double ncx = nw / 2.0;
    double ncy = nh / 2.0;

    for (size_t y = 0; y < nh; y++)
    {
        for (size_t x = 0; x < nw; x++)
        {
            double tx = (x - ncx) * cos(rad) - (y - ncy) * sin(rad) + cx;
            double ty = (x - ncx) * sin(rad) + (y - ncy) * cos(rad) + cy;

            if (tx >= 0 && tx < w && ty >= 0 && ty < h)
            {
                int ix = (int)tx;
                int iy = (int)ty;
                double val = mat_coef(src, iy, ix);
                *mat_coef_ptr(rotated, y, x) = val;
            }
        }
    }

    return rotated;
}

ImageData *rotate_image(ImageData *img, double angle)
{
    int w = img->width;
    int h = img->height;
    Pixel *p = img->pixels;

    double rad = angle * M_PI / 180.0;

    int nw = (int)(fabs(w * cos(rad)) + fabs(h * sin(rad)) + 0.5);
    int nh = (int)(fabs(h * cos(rad)) + fabs(w * sin(rad)) + 0.5);

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

    double cx = w / 2.0;
    double cy = h / 2.0;
    double ncx = nw / 2.0;
    double ncy = nh / 2.0;

    for (int y = 0; y < nh; y++)
    {
        for (int x = 0; x < nw; x++)
        {
            double tx = (x - ncx) * cos(rad) - (y - ncy) * sin(rad) + cx;
            double ty = (x - ncx) * sin(rad) + (y - ncy) * cos(rad) + cy;

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
