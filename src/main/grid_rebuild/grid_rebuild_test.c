#include <stdio.h>

#include "grid_rebuild.h"
#include "location/letters_extraction.h"
#include "solver/grid.h"
#include "utils/utils.h"

int main(void)
{
    printf("=== GRID REBUILD TEST ===\n");

    // Extract letters first (assumes GRID_DIR will be populated)
    if (locate_and_extract_letters_png(LEVEL_1_IMG_2) != 0)
    {
        fprintf(stderr, "Letter extraction failed\n");
        return 1;
    }

    // Rebuild the grid; rows and columns are detected automatically
    Grid *g = grid_rebuild_from_folder_with_model(GRID_DIR,
                                                  "assets/ocr/model/grid.nn");
    if (!g)
    {
        fprintf(stderr, "Grid rebuild failed\n");
        return 1;
    }

    printf("Grid successfully rebuilt (%zu rows x %zu cols):\n", g->height,
           g->width);

    // Print and free
    grid_print(g);
    grid_free(g);

    return 0;
}
