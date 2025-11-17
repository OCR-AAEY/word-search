#include "rotation/hough_lines.h"
#include "utils/math/trigo.h"
#include <err.h>
#include <glib.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

Matrix *create_hough_accumulator_rotation(size_t height, size_t width,
                                 float theta_precision)
{
    if (theta_precision <= 0)
        errx(EXIT_FAILURE, "Theta precision must be strictly positive");

    // Maximum possible distance from the origin of the image (image diagonal)
    double diag = sqrt((double)height * height + (double)width * width);
    size_t r_max = (size_t)ceil(diag);

    // Number of rows = 2*r_max + 1 to include both negative and positive r
    // values
    size_t acc_height = 2 * r_max + 1;

    // Number of columns = number of theta steps (0°..180° exclusive)
    size_t acc_width = (size_t)round(180.0 / theta_precision);

    // columns are the theta values, and lines are the r values
    Matrix *accumulator = mat_create_zero(acc_height, acc_width);

    return accumulator;
}

float populate_hough_lines_top_angle(Matrix *src, Matrix *accumulator,
                                     float theta_precision)
{
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    if (accumulator == NULL)
        errx(EXIT_FAILURE, "The accumulator matrix is NULL");

    size_t height = mat_height(src);
    size_t width = mat_width(src);
    size_t theta_max = mat_width(accumulator);
    // according to the definition of the accumulator height
    size_t r_max = (mat_height(accumulator) - 1) / 2;

    // pre compute cos and sin values, like a cache
    double *cosd_table = malloc(theta_max * sizeof(double));
    double *sind_table = malloc(theta_max * sizeof(double));

    for (size_t t = 0; t < theta_max; t++)
    {
        double theta = t * theta_precision;
        cosd_table[t] = cosd(theta);
        sind_table[t] = sind(theta);
    }

    // initialize the max_theta value and the max_count of votes to keep track
    // of it
    float max_voted_theta = 0.0f;
    size_t max_count = 0;

    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double pixel = *mat_unsafe_coef_ptr(src, h, w);
            // if the pixel is not black, we skip it
            if (pixel != 0)
            {
                continue;
            }
            // for all the theta possible we calculate the r associated
            // this is derived from conversion between cartesian and polar
            // coordinates
            for (size_t theta_index = 0; theta_index < theta_max; theta_index++)
            {
                // theta_max is the maximum index for theta in the accumulator
                // since we have a step of theta_precision, we have :
                double r = (double)w * cosd_table[theta_index] +
                           (double)h * sind_table[theta_index];

                // since r can be at least -r_max, we shift it to an integer
                // to have the index in the accumulator
                // use cast and +0.5 as a round arithmetic operator (faster)
                size_t r_index = (size_t)(r + r_max + 0.5);

                double *accumulator_cell =
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
    return max_voted_theta;
}

float hough_transform_lines_top_angle(Matrix *src, float theta_precision)
{
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    clock_t start, end;
    start = clock();

    Matrix *accumulator = create_hough_accumulator_rotation(
        mat_height(src), mat_width(src), theta_precision);

    end = clock();
    printf("Create accumulator : %lf\n",
           ((double)(end - start)) / CLOCKS_PER_SEC);

    start = clock();
    float max_theta =
        populate_hough_lines_top_angle(src, accumulator, theta_precision);

    end = clock();
    printf("Populate accumulator : %lf\n",
           ((double)(end - start)) / CLOCKS_PER_SEC);
    mat_free(accumulator);

    return max_theta;
}