#include "pretreatment/visualization.h"
#include "utils/math/trigo.h"
#include <err.h>
#include <gtk/gtk.h>
#include <math.h>

void draw_point(cairo_t *cr, float x, float y, float radius)
{
    cairo_arc(cr, x, y, radius, 0, 2 * M_PI);
    cairo_fill(cr);
}

int draw_points_on_img(Point **points, size_t height, size_t width,
                       char *input_filename, char *output_filename)
{
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(input_filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr,
                "draw_points_on_img: Failed to load the image with cairo\n");
        cairo_surface_destroy(surface);
        return EXIT_FAILURE;
    }
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
    return EXIT_SUCCESS;
}

int draw_line(cairo_t *cr, Line *line, int width, int height)
{
    float theta = line->theta;
    float r = line->r;
    float ct = cosd(theta);
    float st = sind(theta);

    // Possible intersections
    float x[2], y[2];
    int i = 0;

    // Left border (x=0)
    if (fabsf(st) > 1e-9f)
    {
        float y0 = r / st;
        // printf("left border : y = %f, height = %i\n", y0, height);
        if (y0 >= 0.0f && y0 <= (float)height)
        {
            x[i] = 0;
            y[i] = y0;
            i++;
        }
    }

    // Right border (x=W)
    if (fabsf(st) > 1e-9f)
    {
        float y1 = (r - (float)width * ct) / st;
        // printf("right border : y = %f, height = %i\n", y1, height);
        if (y1 >= 0.0f && y1 <= (float)height)
        {
            x[i] = width;
            y[i] = y1;
            i++;
        }
    }

    // Top border (y=0)
    if (fabsf(ct) > 1e-9f)
    {
        float x0 = r / ct;
        // printf("top border : x = %f, width = %i\n", x0, width);
        if (x0 >= 0.0f && x0 <= (float)width)
        {
            x[i] = x0;
            y[i] = 0;
            i++;
        }
    }

    // Bottom border (y=H)
    if (fabs(ct) > 1e-9f)
    {
        float x1 = (r - (float)height * st) / ct;
        // printf("bottom border : x = %f, width = %i\n", x1, width);
        if (x1 >= 0.0f && x1 <= (float)width)
        {
            x[i] = x1;
            y[i] = height;
            i++;
        }
    }
    // printf("cos = %f, sin = %f, theta = %f, r = %f\n", ct, st, theta, r);
    if (i != 2)
    {
        fprintf(stderr,
                "draw_line: 2 points not found, got %i, cannot draw line\n", i);
        return EXIT_FAILURE;
    }
    cairo_move_to(cr, x[0], y[0]);
    cairo_line_to(cr, x[1], y[1]);
    cairo_stroke(cr);

    // printf("Draw (%.2f, %.2f) -> (%.2f, %.2f)\n", x[0], y[0], x[1], y[1]);
    return EXIT_SUCCESS;
}

int draw_lines_on_img(Line **lines, size_t line_count, char *input_filename,
                      char *output_filename)
{
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(input_filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr,
                "draw_lines_on_img: Failed to load the image with cairo\n");
        cairo_surface_destroy(surface);
        return EXIT_FAILURE;
    }
    cr = cairo_create(surface);

    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);

    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_set_line_width(cr, 5);

    int status;
    for (size_t i = 0; i < line_count; i++)
    {
        status = draw_line(cr, lines[i], width, height);
        if (status != 0)
        {
            cairo_destroy(cr);
            cairo_surface_destroy(surface);
            return EXIT_FAILURE;
        }
    }

    cairo_surface_write_to_png(surface, output_filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    return EXIT_SUCCESS;
}

int draw_line_cartesian(cairo_t *cr, int x0, int y0, int x1, int y1)
{
    if (cr == NULL)
    {
        fprintf(
            stderr,
            "draw_line_cartesian: Cairo context is NULL, impossible to draw\n");
        return EXIT_FAILURE;
    }
    cairo_move_to(cr, x0, y0);
    cairo_line_to(cr, x1, y1);
    cairo_stroke(cr);
    return EXIT_SUCCESS;
}

int draw_boundingbox(cairo_t *cr, BoundingBox *box)
{
    if (box == NULL)
    {
        fprintf(stderr,
                "draw_boundingbox: Box is NULL, impossible to draw it\n");
        return -1;
    }
    size_t height = box->br.y - box->tl.y + 1;
    size_t width = box->br.x - box->tl.x + 1;

    int status;

    // top line
    status = draw_line_cartesian(cr, box->tl.x, box->tl.y, box->tl.x + width,
                                 box->tl.y);
    if (status != 0)
    {
        fprintf(stderr, "draw_boundingbox: Failed to draw top line\n");
        return -2;
    }
    // left line
    status = draw_line_cartesian(cr, box->tl.x, box->tl.y, box->tl.x,
                                 box->tl.y + height);
    if (status != 0)
    {
        fprintf(stderr, "draw_boundingbox: Failed to draw left line\n");
        return -3;
    }
    // right line
    status = draw_line_cartesian(cr, box->tl.x + width, box->tl.y,
                                 box->tl.x + width, box->tl.y + height);
    if (status != 0)
    {
        fprintf(stderr, "draw_boundingbox: Failed to draw right line\n");
        return -4;
    }
    // bottom line
    status = draw_line_cartesian(cr, box->tl.x, box->tl.y + height,
                                 box->tl.x + width, box->tl.y + height);
    if (status != 0)
    {
        fprintf(stderr, "draw_boundingbox: Failed to draw bottom line\n");
        return -5;
    }
    return EXIT_SUCCESS;
}

int draw_boundingbox_on_img(BoundingBox *box, const char *input_filename,
                            const char *output_filename)
{
    if (box == NULL)
    {
        fprintf(
            stderr,
            "draw_boundingbox_on_img: Box is NULL, impossible to draw it\n");
        return -1;
    }
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(input_filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(
            stderr,
            "draw_boundingbox_on_img: Failed to load the image with cairo\n");
        cairo_surface_destroy(surface);
        return -2;
    }
    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 0, 0.8);
    cairo_set_line_width(cr, 5);

    int status = draw_boundingbox(cr, box);
    if (status != 0)
    {
        fprintf(stderr,
                "draw_boundingbox_on_img: Failed to draw bounding box\n");
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return -3;
    }

    cairo_surface_write_to_png(surface, output_filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    return EXIT_SUCCESS;
}

int draw_boundingboxes(cairo_t *cr, BoundingBox **boxes, size_t nb_boxes)
{
    int status;
    for (size_t i = 0; i < nb_boxes; i++)
    {
        status = draw_boundingbox(cr, boxes[i]);
        if (status != 0)
        {
            fprintf(stderr,
                    "draw_boundingboxes: Failed to draw bounding box\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int draw_boundingboxes_on_img(BoundingBox **boxes, size_t nb_boxes,
                              const char *input_filename,
                              const char *output_filename)
{
    if (boxes == NULL)
    {
        fprintf(stderr, "draw_boundingboxes_on_img: Boxes is NULL, impossible "
                        "to draw them\n");
        return -1;
    }
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(input_filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
    {
        fprintf(stderr, "draw_boundingboxes_on_img: Failed to "
                        "load the image with cairo\n");
        cairo_surface_destroy(surface);
        return -2;
    }

    cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 0, 0, 1);
    cairo_set_line_width(cr, 2);

    int status = draw_boundingboxes(cr, boxes, nb_boxes);
    if (status != 0)
    {
        fprintf(stderr,
                "draw_boundingboxes_on_img: Failed to draw bounding boxes\n");
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return -3;
    }

    cairo_surface_write_to_png(surface, output_filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    return EXIT_SUCCESS;
}

int draw_2d_boundingboxes_on_img(BoundingBox ***boxes, size_t nb_boxes,
                                 size_t *size_array, const char *input_filename,
                                 const char *output_filename)
{
    if (boxes == NULL)
    {
        fprintf(stderr, "draw_2d_boundingboxes_on_img: Boxes is NULL, "
                        "impossible to draw them\n");
        return -1;
    }
    cairo_t *cr;
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_from_png(input_filename);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
    {

        fprintf(stderr, "draw_2d_boundingboxes_on_img: Failed to load the "
                        "image with cairo\n");
        return -2;
    }
    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 0, 0, 1);
    cairo_set_line_width(cr, 2);

    for (size_t i = 0; i < nb_boxes; i++)
    {
        if (boxes[i] == NULL)
        {
            fprintf(stderr, "draw_2d_boundingboxes_on_img: The current array "
                            "of boxes is NULL, impossible to draw them\n");
            return -3;
        }
        draw_boundingboxes(cr, boxes[i], size_array[i]);
    }

    cairo_surface_write_to_png(surface, output_filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    return EXIT_SUCCESS;
}

ImageData *pixel_matrix_to_image(Matrix *matrix)
{
    ImageData *img = malloc(sizeof(ImageData));
    if (img == NULL)
    {
        fprintf(
            stderr,
            "pixel_matrix_to_image: Failed to allocate the ImageData struct\n");
        return NULL;
    }

    size_t height = mat_height(matrix);
    size_t width = mat_width(matrix);
    Pixel *pixels = malloc(height * width * sizeof(Pixel));

    if (pixels == NULL)
    {
        free(img);
        fprintf(stderr,
                "pixel_matrix_to_image: Failed to allocate the pixels array\n");
        return NULL;
    }

    for (size_t h = 0; h < height; h++)
    {
        for (size_t w = 0; w < width; w++)
        {
            float gray_scaled_pixel = mat_coef(matrix, h, w);

            if ((int)gray_scaled_pixel < 0 || (int)gray_scaled_pixel > 255)
            {
                free(img);
                free(pixels);
                fprintf(stderr, "pixel_matrix_to_image: Matrix values must be "
                                "between 0 and 255\n");
                return NULL;
            }

            Pixel *pixel = &pixels[h * width + w];
            // Convert the grayscale float to a uint8_t (with rounding)
            uint8_t gray = (uint8_t)floorf(gray_scaled_pixel + 0.5);
            pixel->r = gray;
            pixel->g = gray;
            pixel->b = gray;
        }
    }
    img->height = height;
    img->width = width;
    img->pixels = pixels;
    return img;
}

int export_matrix(Matrix *src, const char *filename)
{
    ImageData *img = pixel_matrix_to_image(src);
    GdkPixbuf *pixbuf = create_pixbuf_from_image_data(img);
    if (pixbuf == NULL)
        return EXIT_FAILURE;
    GError *error;
    save_pixbuf_to_png(pixbuf, (char *)filename, &error);
    if (error != NULL)
        g_error_free(error);
    g_object_unref(pixbuf);
    free_image(img);
    return EXIT_SUCCESS;
}