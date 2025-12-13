#include "image_loader/image_loading.h"
#include "location/hough_lines_legacy.h"
#include "location/location.h"
#include <cairo.h>

/// @brief Draws a filled circular point on a Cairo surface.
/// @param[in,out] cr Cairo drawing context to render the point on.
/// @param[in] x X-coordinate of the point center.
/// @param[in] y Y-coordinate of the point center.
/// @param[in] radius Radius of the point circle.
void draw_point(cairo_t *cr, float x, float y, float radius);

/// @brief Draws multiple points on an input image and saves the result as a PNG
/// file.
/// @param[in] points 2D array of Point structures containing the coordinates of
/// points to draw.
/// @param[in] height Number of point rows.
/// @param[in] width Number of point columns.
/// @param[in] input_filename Path to the input PNG image.
/// @param[in] output_filename Path to save the output image with drawn points.
/// @returns 0 on success or 1 if the image cannot be loaded.
int draw_points_on_img(Point **points, size_t height, size_t width,
                       char *input_filename, char *output_filename);

/// @brief Draws a line defined in polar form (r, θ) on a Cairo surface within
/// given dimensions.
/// @param[in,out] cr Cairo drawing context to render the line on.
/// @param[in] line Pointer to a Line structure containing the polar parameters.
/// @param[in] width Width of the drawing area.
/// @param[in] height Height of the drawing area.
/// @returns 0 on success or 1 if no valid intersection points can be
/// determined.
int draw_line(cairo_t *cr, Line *line, int width, int height);

/// @brief Draws multiple lines on an input PNG image and saves the result.
/// Loads the image from @p input_filename, draws each line in red with a line
/// width of 5 pixels, and writes the result to @p output_filename.
/// @param[in] lines Array of pointers to Line structures. Each must not be
/// NULL.
/// @param[in] line_count Number of lines in the array.
/// @param[in] input_filename Path to the input PNG image.
/// @param[in] output_filename Path where the output PNG image will be saved.
/// @return 0 on success, or EXIT_FAILURE if the image cannot be loaded or any
/// line fails to draw.
int draw_lines_on_img(Line **lines, size_t line_count, char *input_filename,
                      char *output_filename);

/// @brief Draws a straight line between two points on a Cairo surface.
/// @param[in,out] cr Cairo drawing context to render the line on. Must not be
/// NULL.
/// @param[in] x0 Starting point x-coordinate.
/// @param[in] y0 Starting point y-coordinate.
/// @param[in] x1 Ending point x-coordinate.
/// @param[in] y1 Ending point y-coordinate.
/// @returns 0 on success or 1 if the Cairo context is NULL.
int draw_line_cartesian(cairo_t *cr, int x0, int y0, int x1, int y1);

/// @brief Draws a rectangular bounding box using the given Cairo context.
/// The rectangle is defined by the top-left (tl) and bottom-right (br) corners
/// of the bounding box.
/// @param[in] cr Pointer to the Cairo context. Must not be NULL.
/// @param[in] box Pointer to the BoundingBox to draw. Must not be NULL.
/// @return 0 on success, or a negative error code if drawing any side fails:
///         -1 if box is NULL,
///         -2 if top line drawing fails,
///         -3 if left line drawing fails,
///         -4 if right line drawing fails,
///         -5 if bottom line drawing fails.
int draw_boundingbox(cairo_t *cr, BoundingBox *box);

/// @brief Draws a bounding box on an input image and saves the result as a new
/// PNG file.
/// @param[in] box Pointer to the BoundingBox structure defining the top-left
/// and bottom-right corners.
/// @param[in] input_filename Path to the input PNG image file.
/// @param[in] output_filename Path to save the output PNG image with the drawn
/// bounding box.
/// @returns 0 on success, or an error code if the procedure failed.
int draw_boundingbox_on_img(BoundingBox *box, const char *input_filename,
                            const char *output_filename);

/// @brief Draws multiple bounding boxes using the given Cairo context.
/// Calls draw_boundingbox() for each box in the array.
/// @param[in] cr Pointer to the Cairo context. Must not be NULL.
/// @param[in] boxes Array of pointers to BoundingBox structures. Each must not
/// be NULL.
/// @param[in] nb_boxes Number of bounding boxes in the array.
/// @return 0 on success, or EXIT_FAILURE if drawing any bounding box fails.
int draw_boundingboxes(cairo_t *cr, BoundingBox **boxes, size_t nb_boxes);

/// @brief Draws multiple bounding boxes on an input PNG image and saves the
/// result. Loads the image from @p input_filename, draws each bounding box in
/// blue with a line width of 2 pixels, and writes the result to @p
/// output_filename.
/// @param[in] boxes Array of pointers to BoundingBox structures. Must not be
/// NULL.
/// @param[in] nb_boxes Number of bounding boxes in the array.
/// @param[in] input_filename Path to the input PNG image.
/// @param[in] output_filename Path where the output PNG image will be saved.
/// @return 0 on success, or a negative error code on failure:
///         -1 if boxes is NULL,
///         -2 if the input image cannot be loaded,
///         -3 if drawing the bounding boxes fails.
int draw_boundingboxes_on_img(BoundingBox **boxes, size_t nb_boxes,
                              const char *input_filename,
                              const char *output_filename);

/// @brief Draws multiple sets of bounding boxes on an input PNG image and saves
/// the result. Loads the image from @p input_filename, draws each set of
/// bounding boxes in blue with a line width of 2 pixels, and writes the result
/// to @p output_filename.
/// @param[in] boxes 2D array of pointers to BoundingBox structures. Must not be
/// NULL.
/// @param[in] nb_boxes Number of bounding box arrays.
/// @param[in] size_array Array containing the size of each bounding box array.
/// Must match nb_boxes.
/// @param[in] input_filename Path to the input PNG image.
/// @param[in] output_filename Path where the output PNG image will be saved.
/// @return 0 on success, or a negative error code on failure:
///         -1 if boxes is NULL,
///         -2 if the input image cannot be loaded,
///         -3 if any bounding box array is NULL.
int draw_2d_boundingboxes_on_img(BoundingBox ***boxes, size_t nb_boxes,
                                 size_t *size_array, const char *input_filename,
                                 const char *output_filename);

/// @brief Exports a matrix as a PNG image file.
/// Converts the matrix to image data, creates a GdkPixbuf, and saves it as a
/// PNG at the specified filename.
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] filename Path where the PNG image will be saved.
/// @return 0 on success, or EXIT_FAILURE if the export fails.
int export_matrix(Matrix *src, const char *filename);

/// @brief Converts a grayscale matrix to an RGB image.
/// @param[in] matrix Pointer to the input grayscale matrix (values 0.0–255.0).
/// @return Pointer to a newly allocated ImageData containing RGB pixels or NULL
/// on failure.
ImageData *pixel_matrix_to_image(Matrix *matrix);