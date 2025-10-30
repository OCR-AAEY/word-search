#include "bounding_boxes/hough_lines.h"
#include "bounding_boxes/location.h"
#include <cairo.h>

/// @brief Draws a filled circular point on a Cairo surface.
/// @param[in,out] cr Cairo drawing context to render the point on.
/// @param[in] x X-coordinate of the point center.
/// @param[in] y Y-coordinate of the point center.
/// @param[in] radius Radius of the point circle.
void draw_point(cairo_t *cr, double x, double y, double radius);

/// @brief Draws multiple points on an input image and saves the result as a PNG
/// file.
/// @param[in] points 2D array of Point structures containing the coordinates of
/// points to draw.
/// @param[in] height Number of point rows.
/// @param[in] width Number of point columns.
/// @param[in] input_filename Path to the input PNG image.
/// @param[in] output_filename Path to save the output image with drawn points.
/// @throw Exits the program if the image cannot be loaded.
void draw_points_on_img(Point **points, size_t height, size_t width,
                        char *input_filename, char *output_filename);

/// @brief Draws a line defined in polar form (r, θ) on a Cairo surface within
/// given dimensions.
/// @param[in,out] cr Cairo drawing context to render the line on.
/// @param[in] line Pointer to a Line structure containing the polar parameters.
/// @param[in] width Width of the drawing area.
/// @param[in] height Height of the drawing area.
/// @throw Exits the program if valid intersection points cannot be determined.
void draw_line(cairo_t *cr, Line *line, int width, int height);

/// @brief Draws multiple lines on an input image and saves the result as a PNG
/// file.
/// @param[in] lines Array of Line structures to be drawn.
/// @param[in] line_count Number of lines in the array.
/// @param[in] input_filename Path to the input PNG image.
/// @param[in] output_filename Path to save the output image with drawn lines.
/// @throw Exits the program if the image cannot be loaded.
void draw_lines_on_img(Line **lines, size_t line_count, char *input_filename,
                       char *output_filename);

/// @brief Draws a straight line between two points on a Cairo surface.
/// @param[in,out] cr Cairo drawing context to render the line on. Must not be
/// NULL.
/// @param[in] x0 Starting point x-coordinate.
/// @param[in] y0 Starting point y-coordinate.
/// @param[in] x1 Ending point x-coordinate.
/// @param[in] y1 Ending point y-coordinate.
/// @throw Exits the program if the Cairo context is NULL.
void draw_line_cartesian(cairo_t *cr, int x0, int y0, int x1, int y1);

void draw_boundingbox(cairo_t *cr, BoundingBox *box);

/// @brief Draws a bounding box on an input image and saves the result as a new
/// PNG file.
/// @param[in] box Pointer to the BoundingBox structure defining the top-left
/// and bottom-right corners.
/// @param[in] input_filename Path to the input PNG image file.
/// @param[in] output_filename Path to save the output PNG image with the drawn
/// bounding box.
/// @throw Exits the program if the box is NULL or if the image fails to load.
void draw_boundingbox_on_img(BoundingBox *box, const char *input_filename,
                             const char *output_filename);

void draw_boundingboxes(cairo_t *cr, BoundingBox **boxes, size_t nb_boxes);

void draw_boundingboxes_on_img(BoundingBox **boxes, size_t nb_boxes,
                               const char *input_filename,
                               const char *output_filename);

void draw_2d_boundingboxes_on_img(BoundingBox ***boxes, size_t nb_boxes,
                                  size_t *size_array,
                                  const char *input_filename,
                                  const char *output_filename);