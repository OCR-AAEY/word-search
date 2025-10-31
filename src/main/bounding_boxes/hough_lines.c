#include "bounding_boxes/hough_lines.h"
#include "utils/utils.h"
#include <err.h>
#include <glib.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

Matrix *create_hough_accumulator(size_t height, size_t width,
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
    Matrix *accumulator = mat_create_empty(acc_height, acc_width);

    return accumulator;
}

void populate_hough_lines(Matrix *src, Matrix *accumulator,
                          double theta_precision, size_t *max_count)
{
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");

    if (accumulator == NULL)
        errx(EXIT_FAILURE, "The accumulator matrix is NULL");

    if (max_count == NULL)
        errx(EXIT_FAILURE, "The max_count output parameter is NULL");

    size_t height = mat_height(src);
    size_t width = mat_width(src);
    size_t theta_max = mat_width(accumulator);
    // according to the definition of the accumulator height
    size_t r_max = (mat_height(accumulator) - 1) / 2;
    *max_count = 0;

    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double pixel = mat_coef(src, h, w);
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
                double r =
                    (double)w * cosd((double)theta_index * theta_precision) +
                    (double)h * sind((double)theta_index * theta_precision);

                // since r can be at least -r_max, we shift it to an integer
                // to have the index in the accumulator
                size_t r_index = (size_t)round(r + (double)r_max);

                double *accumulator_cell =
                    mat_coef_addr(accumulator, r_index, theta_index);
                (*accumulator_cell)++;

                // we keep track of the maximum value of the accumulator
                if (*accumulator_cell > *max_count)
                    *max_count = *accumulator_cell;
            }
        }
    }
}

void statistics_on_accumulator(Matrix *accumulator, double *stddev_out,
                               double *mean_out)
{
    if (stddev_out == NULL || mean_out == NULL)
        errx(EXIT_FAILURE, "Got null out parameters : stddev or mean");
    size_t height = mat_height(accumulator);
    size_t width = mat_width(accumulator);
    size_t N = height * width;
    double sum = 0;
    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            sum += mat_coef(accumulator, h, w);
        }
    }
    *mean_out = sum / N;

    double var_sum = 0;
    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            double diff = mat_coef(accumulator, h, w) - *mean_out;
            var_sum += diff * diff;
        }
    }

    *stddev_out = sqrt(var_sum / N);
}

Line **extract_hough_lines(Matrix *accumulator, size_t threshold,
                           double theta_precision, size_t *line_count)
{
    if (accumulator == NULL)
        errx(EXIT_FAILURE, "The accumulator matrix is NULL");
    if (line_count == NULL)
        errx(EXIT_FAILURE, "The line_count output parameter is NULL");
    if (threshold == 0)
        errx(EXIT_FAILURE, "Threshold should not be 0 because it would "
                           "generate to much noise");

    size_t theta_max = mat_width(accumulator);
    size_t r_max = (mat_height(accumulator) - 1) / 2;

    size_t max_lines = 20; // initial size of the lines alloc
    Line **lines = malloc(max_lines * sizeof(Line *));
    *line_count = 0;

    for (size_t r = 0; r < 2 * r_max + 1; r++)
    {
        for (size_t theta_index = 0; theta_index < theta_max; theta_index++)
        {
            double count = mat_coef(accumulator, r, theta_index);
            // ignore values that are below the threshold
            if (count < threshold)
                continue;
            // increase the size of the array if the max number of lines is
            // reached
            if (*line_count == max_lines)
            {
                max_lines += 10;
                Line **tmp = realloc(lines, max_lines * sizeof(Line *));
                if (tmp == NULL)
                {
                    free_lines(lines, *line_count);
                    errx(EXIT_FAILURE, "Reallocation of lines array failed");
                }
                lines = tmp;
            }
            // add the line to the list of lines
            Line *line = malloc(sizeof(Line));
            if (line == NULL)
            {
                free_lines(lines, *line_count);
                errx(EXIT_FAILURE, "Line allocation failed");
            }
            lines[(*line_count)++] = line;
            line->r = (double)r - (double)r_max;
            line->theta = (double)theta_index * theta_precision;
        }
    }
    return lines;
}

Line **hough_lines_NMS(Line **lines, size_t *line_count, double delta_r,
                       double delta_theta)
{
    if (lines == NULL)
        errx(EXIT_FAILURE, "The Lines array is NULL");
    if (line_count == NULL || *line_count == 0)
        errx(EXIT_FAILURE, "The line count is NULL or 0");
    if (delta_r < 0 || delta_theta < 0)
        errx(EXIT_FAILURE, "delta_r and delta_theta must not be negative");

    int *suppressed = calloc(*line_count, sizeof(int));

    for (size_t i = 0; i < *line_count; i++)
    {
        // ignore if already suppressed line
        if (suppressed[i])
            continue;

        Line *li = lines[i];

        for (size_t j = i + 1; j < *line_count; j++)
        {
            // ignore if already suppressed line
            if (suppressed[j])
                continue;
            Line *lj = lines[j];

            // mark as suppressed the lines that are too close in r and theta
            // considering they are the same
            if (ABS(round(lj->r - li->r)) < delta_r &&
                ABS(round(lj->theta - li->theta)) < delta_theta)
            {
                suppressed[j] = TRUE;
            }
        }
    }

    // overwrite the lines with the filtered ones
    size_t rewrite_index = 0;
    for (size_t i = 0; i < *line_count; i++)
    {
        if (!suppressed[i])
            lines[rewrite_index++] = lines[i];
        else
        {
            // free the ones that are suppressed
            free(lines[i]);
        }
    }
    // the new number of lines is the number of non suppressed lines
    *line_count = rewrite_index;
    free(suppressed);
    return lines;
}

Line **hough_transform_lines(Matrix *src, float theta_precision, double delta_r,
                             double delta_theta, size_t *size_out)
{
    if (src == NULL)
        errx(EXIT_FAILURE, "The source matrix is NULL");
    if (size_out == NULL)
        errx(EXIT_FAILURE, "The size output parameter is NULL");

    Matrix *accumulator = create_hough_accumulator(
        mat_height(src), mat_width(src), theta_precision);

    size_t max_acc;
    populate_hough_lines(src, accumulator, theta_precision, &max_acc);

    // double stddev, mean;
    // statistics_on_accumulator(accumulator, &stddev, &mean);

    // double k = 5;
    // float threshold = mean + k*stddev;
    float threshold = max_acc * 0.7;
    Line **lines =
        extract_hough_lines(accumulator, threshold, theta_precision, size_out);

    // filter lines that are too similar
    lines = hough_lines_NMS(lines, size_out, delta_r, delta_theta);

    mat_free(accumulator);

    return lines;
}

void print_lines(Line **lines, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        double theta = lines[i]->theta;
        double r = lines[i]->r;

        printf("Line %zu : (%f, %f)\n", i, r, theta);
    }
}

void free_lines(Line **lines, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        free(lines[i]);
    }
    free(lines);
}

void insert_line_in_group(Line *line, Line ***lines_group, size_t *lines_count,
                          size_t *max_group)
{
    if (*lines_count == *max_group)
    {
        *max_group += 10;
        *lines_group = realloc(*lines_group, *max_group * sizeof(Line *));
        if (*lines_group == NULL)
            errx(EXIT_FAILURE, "Failed to reallocate lines group");
    }
    (*lines_group)[(*lines_count)++] = line;
}

void split_lines(Line **lines, size_t line_count, Line ***lines_1,
                 size_t *lines_1_count, Line ***lines_2, size_t *lines_2_count)
{
    if (lines == NULL)
        errx(EXIT_FAILURE, "The lines array is NULL");
    if (lines_1 == NULL)
        errx(EXIT_FAILURE, "The lines_1 output parameter is NULL");
    if (lines_2 == NULL)
        errx(EXIT_FAILURE, "The lines_2 output parameter is NULL");
    if (line_count == 0)
        errx(EXIT_FAILURE, "There is no lines to sort");
    if (lines_1_count == NULL)
        errx(EXIT_FAILURE, "The lines_1_count output parameter is NULL");
    if (lines_2_count == NULL)
        errx(EXIT_FAILURE, "The lines_2_count output parameter is NULL");

    size_t max_1 = line_count / 2;
    size_t max_2 = line_count / 2;
    *lines_1_count = 0;
    *lines_2_count = 0;
    *lines_1 = malloc(max_1 * sizeof(Line *));
    *lines_2 = malloc(max_2 * sizeof(Line *));

    for (size_t i = 0; i < line_count; i++)
    {
        Line *li = lines[i];
        if (*lines_1_count == 0 || li->theta == (*lines_1)[0]->theta)
        {
            insert_line_in_group(li, lines_1, lines_1_count, &max_1);
        }
        else if (*lines_2_count == 0 || li->theta == (*lines_2)[0]->theta)
        {
            insert_line_in_group(li, lines_2, lines_2_count, &max_2);
        }
        else
        {
            if (li->theta == (*lines_1)[0]->theta)
            {
                insert_line_in_group(li, lines_1, lines_1_count, &max_1);
            }
            else if (li->theta == (*lines_2)[0]->theta)
            {
                insert_line_in_group(li, lines_2, lines_2_count, &max_2);
            }
            else
            {
                printf("theta1 = %f, theta2 = %f, got %f\n",
                       (*lines_1)[0]->theta, (*lines_2)[0]->theta, li->theta);
                errx(EXIT_FAILURE,
                     "There are 3 lines angles : impossible to split in 2");
            }
        }
    }
    // printf("theta1 = %f, theta2 = %f\n",
    //                    (*lines_1)[0]->theta, (*lines_2)[0]->theta);
}

Point **extract_intersection_points(Line **lines, size_t line_count,
                                    size_t *height_out, size_t *width_out)
{
    if (lines == NULL)
        errx(EXIT_FAILURE, "The lines array is NULL");
    if (height_out == NULL)
        errx(EXIT_FAILURE, "The height output parameter is NULL");
    if (width_out == NULL)
        errx(EXIT_FAILURE, "The width output parameter is NULL");

    Line **lines_1;
    Line **lines_2;
    size_t lines_1_count;
    size_t lines_2_count;

    split_lines(lines, line_count, &lines_1, &lines_1_count, &lines_2,
                &lines_2_count);

    *height_out = lines_1_count;
    *width_out = lines_2_count;

    Point **points = malloc(lines_1_count * sizeof(Point *));

    if (points == NULL)
        errx(EXIT_FAILURE, "Memory allocation failed for points");

    for (size_t i = 0; i < lines_1_count; i++)
    {
        points[i] = malloc(lines_2_count * sizeof(Point));
        if (points[i] == NULL)
            errx(EXIT_FAILURE, "Memory allocation failed for points row");
    }

    for (size_t i_1 = 0; i_1 < lines_1_count; i_1++)
    {
        Line *l1 = lines_1[i_1];
        for (size_t i_2 = 0; i_2 < lines_2_count; i_2++)
        {
            Line *l2 = lines_2[i_2];
            if (l1->theta == l2->theta)
                errx(EXIT_FAILURE, "Both group of lines have the same angle, "
                                   "there is no intersection");

            Point intersection;
            intersection.x =
                (int)round((l2->r * sind(l1->theta) - l1->r * sind(l2->theta)) /
                           sind(l1->theta - l2->theta));
            intersection.y =
                (int)round((l1->r * cosd(l2->theta) - l2->r * cosd(l1->theta)) /
                           sind(l1->theta - l2->theta));
            points[i_1][i_2] = intersection;
        }
    }

    free(lines_1);
    free(lines_2);
    return points;
}

void print_points(Point **points, size_t height, size_t width)
{
    printf("Found %zu points\n", height * width);
    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            printf("Point (%zu, %zu) : (%i, %i)\n", h, w, points[h][w].x,
                   points[h][w].y);
        }
    }
}

void free_points(Point **points, size_t height)
{
    for (size_t h = 0; h < height; h++)
    {
        free(points[h]);
    }
    free(points);
}