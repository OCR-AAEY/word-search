#include "rotation.h"
#include "rotation/hough_lines.h"
#include "utils/math/trigo.h"
#include <err.h>

Matrix *rotate_matrix(const Matrix *src, double angle)
{
    if (src == NULL)
    {
        errx(EXIT_FAILURE, "rotate_matrix: src matrix is NULL");
    }

    size_t w = mat_width(src);
    size_t h = mat_height(src);

    double cos_angle = cosd(angle);
    double sin_angle = sind(angle);

    size_t nw = (size_t)(fabs((double)w * cos_angle) +
                         fabs((double)h * sin_angle) + 0.5);
    size_t nh = (size_t)(fabs((double)h * cos_angle) +
                         fabs((double)w * sin_angle) + 0.5);

    Matrix *rotated = mat_create_zero(nh, nw);

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
            double tx = (x - ncx) * cos_angle - (y - ncy) * sin_angle + cx;
            double ty = (x - ncx) * sin_angle + (y - ncy) * cos_angle + cy;

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

    double cos_angle = cosd(angle);
    double sin_angle = sind(angle);

    int nw = (int)(fabs(w * cos_angle) + fabs(h * sin_angle) + 0.5);
    int nh = (int)(fabs(h * cos_angle) + fabs(w * sin_angle) + 0.5);

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
            double tx = (x - ncx) * cos_angle - (y - ncy) * sin_angle + cx;
            double ty = (x - ncx) * sin_angle + (y - ncy) * cos_angle + cy;

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

Matrix *auto_rotate_matrix(Matrix *img)
{
    float theta_angle = hough_transform_lines_top_angle(img, 1.0f);

    float rotation_angle = fmodf(theta_angle, 90);
    if (rotation_angle > 45)
    {
        rotation_angle = rotation_angle - 90;
    }
    printf("Rotation applied :%f\n", rotation_angle);

    Matrix *rotated = rotate_matrix(img, rotation_angle);
    return rotated;
}