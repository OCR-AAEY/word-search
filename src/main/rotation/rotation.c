#include "rotation.h"
#include "rotation/hough_lines.h"
#include "utils/math/trigo.h"
#include <err.h>

Matrix *rotate_matrix(const Matrix *src, float angle)
{
    if (src == NULL)
    {
        errx(EXIT_FAILURE, "rotate_matrix: src matrix is NULL");
    }

    size_t w = mat_width(src);
    size_t h = mat_height(src);

    float cos_angle = cosd(angle);
    float sin_angle = sind(angle);

    size_t nw = (size_t)(fabs((float)w * cos_angle) +
                         fabs((float)h * sin_angle) + 0.5);
    size_t nh = (size_t)(fabs((float)h * cos_angle) +
                         fabs((float)w * sin_angle) + 0.5);

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
            float tx = ((float)x - ncx) * cos_angle - ((float)y - ncy) * sin_angle + cx;
            float ty = ((float)x - ncx) * sin_angle + ((float)y - ncy) * cos_angle + cy;

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

    float cos_angle = cosd(angle);
    float sin_angle = sind(angle);

    int nw = (int)(fabs((float)w * cos_angle) + fabs((float)h * sin_angle) + 0.5);
    int nh = (int)(fabs((float)h * cos_angle) + fabs((float)w * sin_angle) + 0.5);

    Pixel *np = calloc(nw * nh, sizeof(Pixel));
    if (np == NULL)
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
            float tx = ((float)x - ncx) * cos_angle - ((float)y - ncy) * sin_angle + cx;
            float ty = ((float)x - ncx) * sin_angle + ((float)y - ncy) * cos_angle + cy;

            if (tx >= 0.0f && tx < (float)w && ty >= 0.0f && ty < (float)h)
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
    float theta_angle = hough_transform_find_peak_angle(img, 1.0f);

    float rotation_angle = fmodf(theta_angle, 90.0f);
    if (rotation_angle > 45)
    {
        rotation_angle = rotation_angle - 90.0f;
    }
    // printf("Rotation applied :%f\n", rotation_angle);

    Matrix *rotated = rotate_matrix(img, rotation_angle);
    return rotated;
}