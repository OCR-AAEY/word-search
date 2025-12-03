#include "location/location_grid.h"
#include "extract_char/extract_char.h"
#include "matrix/matrix.h"
#include "utils/utils.h"
#include <err.h>
#include <stdio.h>
#include <string.h>

void extract_grid_cells(Matrix *src, Point **points, size_t height,
                        size_t width)
{
    for (size_t h = 0; h < height - 1; h++)
    {
        for (size_t w = 0; w < width - 1; w++)
        {
            size_t filename_size =
                snprintf(NULL, 0, "%s/(%zu_%zu).png", GRID_DIR, h, w);
            char filename[filename_size + 1];
            sprintf(filename, "%s/(%zu_%zu).png", GRID_DIR, h, w);
            // printf("%s (%i, %i) to (%i, %i)\n", filename, points[h][w].x,
            // points[h][w].y,
            //                   points[h + 1][w + 1].x, points[h + 1][w +
            //                   1].y);
            save_image_region(src, filename, points[h][w].x, points[h][w].y,
                              points[h + 1][w + 1].x, points[h + 1][w + 1].y);
        }
    }
}

BoundingBox *get_bounding_box_grid(Point **points, size_t height, size_t width)
{
    if (height == 0 && width == 0)
        errx(EXIT_FAILURE,
             "Cannot find grid if there is no intersection points");
    BoundingBox *box = malloc(sizeof(BoundingBox));
    // Point top_left = (Point) {.x = points[0][0].x, .y = points[0][0].y};
    // Point bottom_right = (Point) {.x = points[height-1][width-1].x, .y =
    // points[height-1][width-1].y};
    box->tl = points[0][0];
    box->br = points[height - 1][width - 1];
    return box;
}