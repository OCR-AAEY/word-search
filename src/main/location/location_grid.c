#include "location/location_grid.h"
#include "extract_char/extract_char.h"
#include "matrix/matrix.h"
#include "utils/utils.h"
#include <err.h>
#include <stdio.h>
#include <string.h>

int extract_grid_cells(Matrix *src, Point **points, size_t height, size_t width)
{
    const int PADDING = 5;
    int status;

    for (size_t h = 0; h < height - 1; h++)
    {
        for (size_t w = 0; w < width - 1; w++)
        {
            // Coordonnées originales
            int x1 = points[h][w].x;
            int y1 = points[h][w].y;
            int x2 = points[h + 1][w + 1].x;
            int y2 = points[h + 1][w + 1].y;

            // Application du padding
            int px1 = x1 + PADDING;
            int py1 = y1 + PADDING;
            int px2 = x2 - PADDING;
            int py2 = y2 - PADDING;

            // Validation de la région après padding
            if (px1 >= px2 || py1 >= py2)
            {
                fprintf(stderr,
                        "extract_grid_cells: invalid region after padding "
                        "(%zu, %zu): (%d, %d) -> (%d, %d)\n",
                        h, w, px1, py1, px2, py2);
                return EXIT_FAILURE;
            }

            // Construction du nom de fichier
            size_t filename_size =
                snprintf(NULL, 0, "%s/(%zu_%zu).png", GRID_DIR, h, w);
            char filename[filename_size + 1];
            sprintf(filename, "%s/(%zu_%zu).png", GRID_DIR, h, w);

            // Sauvegarde de la région
            status = save_image_region(src, filename, px1, py1, px2, py2);

            if (status != 0)
            {
                fprintf(
                    stderr,
                    "extract_grid_cells: Failed to save grid cell (%zu,%zu)\n",
                    h, w);
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}

BoundingBox *get_bounding_box_grid(Point **points, size_t height, size_t width)
{
    if (height == 0 && width == 0)
    {
        fprintf(stderr, "get_bounding_box_grid: Cannot find grid if there is "
                        "no intersection points\n");
        return NULL;
    }
    BoundingBox *box = malloc(sizeof(BoundingBox));
    if (box == NULL)
    {
        fprintf(stderr, "get_bounding_box_grid: Failed allocation box\n");
        return NULL;
    }
    // Point top_left = (Point) {.x = points[0][0].x, .y = points[0][0].y};
    // Point bottom_right = (Point) {.x = points[height-1][width-1].x, .y =
    // points[height-1][width-1].y};
    box->tl = points[0][0];
    box->br = points[height - 1][width - 1];
    return box;
}