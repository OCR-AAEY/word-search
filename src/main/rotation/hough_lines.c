#include "rotation/hough_lines.h"
#include "utils/math/trigo.h"
#include <err.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

Matrix *create_hough_accumulator_rotation(size_t height, size_t width,
                                          float theta_precision)
{
    if (theta_precision <= 0.0f)
    {
        fprintf(stderr, "Theta precision must be strictly positive\n");
        return NULL;
    }

    // Maximum possible distance from the origin of the image (image diagonal)
    float diag = sqrt((float)height * height + (float)width * width);
    size_t r_max = (size_t)ceil(diag);

    // Number of rows = 2*r_max + 1 to include both negative and positive r
    // values
    size_t acc_height = 2 * r_max + 1;

    // Number of columns = number of theta steps (0°..180° exclusive)
    size_t acc_width = (size_t)round(180.0f / theta_precision);

    // columns are the theta values, and lines are the r values
    Matrix *accumulator = mat_create_zero(acc_height, acc_width);

    return accumulator;
}

int populate_acc_find_peak_theta(Matrix *src, Matrix *accumulator,
                                 float theta_precision, float *out_theta)
{
    if (src == NULL)
        return -1;

    if (accumulator == NULL)
        return -2;

    if (out_theta == NULL)
        return -3;

    if (theta_precision <= 0.0f)
        return -4;

    size_t height = mat_height(src);
    size_t width = mat_width(src);
    size_t theta_index_max = mat_width(accumulator);
    // according to the definition of the accumulator height
    size_t r_max = (mat_height(accumulator) - 1) / 2;

    // pre compute cos and sin values, like a cache
    float *cosd_table = malloc(theta_index_max * sizeof(float));
    float *sind_table = malloc(theta_index_max * sizeof(float));

    if (!cosd_table || !sind_table)
    {
        free(cosd_table);
        free(sind_table);
        return -5;
    }

    for (size_t theta_index = 0; theta_index < theta_index_max; theta_index++)
    {
        float theta = (float)theta_index * theta_precision;
        cosd_table[theta_index] = cosd(theta);
        sind_table[theta_index] = sind(theta);
    }

    // initialize the max_theta value and the max_count of votes to keep track
    // of it
    float max_voted_theta = 0.0f;
    size_t max_count = 0;

    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            const float pixel = *mat_unsafe_coef_ptr(src, h, w);
            // if the pixel is not black, we skip it
            if (pixel != 0)
                continue;

            // for all the theta possible we calculate the r associated
            // this is derived from conversion between cartesian and polar
            // coordinates
            for (size_t theta_index = 0; theta_index < theta_index_max;
                 theta_index++)
            {
                float r = (float)w * cosd_table[theta_index] +
                          (float)h * sind_table[theta_index];

                // since r can be at least -r_max, we shift it to an integer
                // to have the index in the accumulator
                // use cast and +0.5 as a round arithmetic operator (faster)
                size_t r_index = (size_t)(r + r_max + 0.5);

                float *accumulator_cell =
                    mat_unsafe_coef_ptr(accumulator, r_index, theta_index);
                (*accumulator_cell)++;

                // we keep track of the maximum value of the accumulator
                if (*accumulator_cell > max_count)
                {
                    max_count = *accumulator_cell;
                    max_voted_theta = (float)theta_index * theta_precision;
                }
            }
        }
    }

    free(cosd_table);
    free(sind_table);

    *out_theta = max_voted_theta;
    return 0;
}

int hough_transform_find_peak_angle(Matrix *src, float theta_precision,
                                    float *out_angle)
{
    if (src == NULL)
        return -1;

    if (out_angle == NULL)
        return -2;

    Matrix *accumulator = create_hough_accumulator_rotation(
        mat_height(src), mat_width(src), theta_precision);

    if (accumulator == NULL)
        return -3;

    float max_theta;

    int status = populate_acc_find_peak_theta(src, accumulator, theta_precision,
                                              &max_theta);
    if (status != 0)
        return -4;

    mat_free(accumulator);

    *out_angle = max_theta;

    return 0;
}
