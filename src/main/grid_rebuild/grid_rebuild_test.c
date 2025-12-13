#include <stdio.h>
#include <stdlib.h>

#include "grid_rebuild.h"
#include "location/letters_extraction.h"
#include "solver/grid.h"
#include "utils/utils.h"

int main(void)
{
    printf("=== GRID REBUILD TEST ===\n");

    if (locate_and_extract_letters_png(LEVEL_1_IMG_1) != EXIT_SUCCESS)
    {
        fprintf(stderr, "Letter extraction failed\n");
        return EXIT_FAILURE;
    }

    Grid *g = grid_rebuild_from_folder_with_model(GRID_DIR, 17, 17,
                                                  "assets/ocr/model/real.nn");

    if (!g)
    {
        fprintf(stderr, "Grid rebuild failed\n");
        return EXIT_FAILURE;
    }

    grid_print(g);
    grid_free(g);

    return EXIT_SUCCESS;
}
