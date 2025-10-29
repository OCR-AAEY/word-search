#include "bounding_boxes/visualization.h"
#include <err.h>
#include <gtk/gtk.h>
#include <math.h>

void draw_point(cairo_t *cr, double x, double y, double radius)
{
    cairo_arc(cr, x, y, radius, 0, 2 * M_PI);
    cairo_fill(cr);
}

void draw_points_on_img(Point **points, size_t height, size_t width,
                        char *input_filename, char *output_filename)
{
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(input_filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
        errx(EXIT_FAILURE, "Failed to load the image with cairo");
    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 0, 1, 0);
    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            draw_point(cr, points[h][w].x, points[h][w].y, 7);
        }
    }

    cairo_surface_write_to_png(surface, output_filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

void draw_line(cairo_t *cr, Line *line, int width, int height)
{
    double theta = line->theta;
    double r = line->r;
    // if (r < 0)
    // {
    //     r = -r;
    //     theta = fmod(theta + 180, 360);
    // }
    double ct = cosd(theta);
    double st = sind(theta);

    // Possible intersections
    double x[2], y[2];
    int i = 0;

    // Left border (x=0)
    if (fabs(st) > 1e-9)
    {
        double y0 = r / st;
        // printf("left border : y = %f, height = %i\n", y0, height);
        if (y0 >= 0 && y0 <= height)
        {
            x[i] = 0;
            y[i] = y0;
            i++;
        }
    }

    // Right border (x=W)
    if (fabs(st) > 1e-9)
    {
        double y1 = (r - width * ct) / st;
        // printf("right border : y = %f, height = %i\n", y1, height);
        if (y1 >= 0 && y1 <= height)
        {
            x[i] = width;
            y[i] = y1;
            i++;
        }
    }

    // Top border (y=0)
    if (fabs(ct) > 1e-9)
    {
        double x0 = r / ct;
        // printf("top border : x = %f, width = %i\n", x0, width);
        if (x0 >= 0 && x0 <= width)
        {
            x[i] = x0;
            y[i] = 0;
            i++;
        }
    }

    // Bottom border (y=H)
    if (fabs(ct) > 1e-9)
    {
        double x1 = (r - height * st) / ct;
        // printf("bottom border : x = %f, width = %i\n", x1, width);
        if (x1 >= 0 && x1 <= width)
        {
            x[i] = x1;
            y[i] = height;
            i++;
        }
    }
    // printf("cos = %f, sin = %f, theta = %f, r = %f\n", ct, st, theta, r);
    if (i != 2)
        errx(EXIT_FAILURE, "2 points not found, got %i, cannot draw line", i);
    cairo_move_to(cr, x[0], y[0]);
    cairo_line_to(cr, x[1], y[1]);
    cairo_stroke(cr);

    // printf("Draw (%.2f, %.2f) -> (%.2f, %.2f)\n", x[0], y[0], x[1], y[1]);
}

void draw_lines_on_img(Line **lines, size_t line_count, char *input_filename,
                       char *output_filename)
{
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(input_filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
        errx(EXIT_FAILURE, "Failed to load the image with cairo");
    cr = cairo_create(surface);

    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);

    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_set_line_width(cr, 5);
    for (size_t i = 0; i < line_count; i++)
    {
        draw_line(cr, lines[i], width, height);
    }

    cairo_surface_write_to_png(surface, output_filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

void draw_line_cartesian(cairo_t *cr, int x0, int y0, int x1, int y1)
{
    if (cr == NULL)
        errx(EXIT_FAILURE, "Cairo context is NULL, impossible to draw");
    cairo_move_to(cr, x0, y0);
    cairo_line_to(cr, x1, y1);
    cairo_stroke(cr);
}

void draw_boundingbox_on_img(BoundingBox *box, const char *input_filename,
                             const char *output_filename)
{
    if (box == NULL)
        errx(EXIT_FAILURE, "Box is NULL, impossible to draw it");
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(input_filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
        errx(EXIT_FAILURE, "Failed to load the image with cairo");
    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 0, 0.8);
    cairo_set_line_width(cr, 5);

    size_t height = box->br.y - box->tl.y + 1;
    size_t width = box->br.x - box->tl.x + 1;

    // top line
    draw_line_cartesian(cr, box->tl.x, box->tl.y, box->tl.x + width, box->tl.y);
    // left line
    draw_line_cartesian(cr, box->tl.x, box->tl.y, box->tl.x,
                        box->tl.y + height);
    // right line
    draw_line_cartesian(cr, box->tl.x + width, box->tl.y, box->tl.x + width,
                        box->tl.y + height);
    // bottom line
    draw_line_cartesian(cr, box->tl.x, box->tl.y + height, box->tl.x + width,
                        box->tl.y + height);

    cairo_surface_write_to_png(surface, output_filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}
