#include "bounding_boxes/visualization.h"
#include <cairo.h>
#include <err.h>
#include <gtk/gtk.h>
#include <math.h>

void draw_point(cairo_t *cr, double x, double y)
{
    cairo_arc(cr, x, y, 100, 0, 2 * M_PI);
    cairo_stroke(cr);
}

void draw_line(cairo_t *cr, Line *line, int width, int height)
{
    int offset = (int)ceil(sqrt(height * height + width * width));

    double theta = line->theta;
    double r = line->r;

    double x_0 = r * cosd(theta);
    double y_0 = r * sind(theta);
    draw_point(cr, x_0, y_0);
    printf("For line (%f, %f) : x_0 = %f, y_0 = %f\n", r, theta, x_0, y_0);
    double dx = x_0;
    double dy = -y_0;

    // Normalize direction to avoid huge coordinates
    double norm = sqrt(dx * dx + dy * dy);
    dx /= norm;
    dy /= norm;

    double x_1 = x_0 + offset * dx;
    double y_1 = y_0 + offset * dy;

    double x_2 = x_0 - offset * dx;
    double y_2 = y_0 - offset * dy;

    cairo_move_to(cr, x_1, y_1);
    cairo_line_to(cr, x_2, y_2);
    printf("Line : (%.2f, %.2f) -> (%.2f, %.2f)\n", x_1, y_1, x_2, y_2);
}

void draw_lines_on_img(Line **lines, size_t line_count, char *filename)
{
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
        errx(EXIT_FAILURE, "Failed to load the image with cairo");
    cr = cairo_create(surface);

    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);

    cairo_set_source_rgb(cr, 1, 0, 0);
    for (size_t i = 0; i < line_count; i++)
    {
        draw_line(cr, lines[i], width, height);
        cairo_set_line_width(cr, 2);
        cairo_stroke(cr);
    }
    char *result_filename = malloc(128 * sizeof(char));
    sprintf(result_filename, "lines_%s", filename);
    cairo_surface_write_to_png(surface, result_filename);
    free(result_filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}