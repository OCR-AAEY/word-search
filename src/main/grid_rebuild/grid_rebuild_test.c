#include <stdio.h>
#include <stdlib.h>

#include "grid_rebuild.h"
#include "solver/grid.h"

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        fprintf(stderr, "Usage: %s <model_path> <cells_folder> <rows> <cols>\n",
                argv[0]);
        fprintf(stderr, "Example: %s assets/model.bin grid_rebuild 15 15\n",
                argv[0]);
        return 1;
    }

    const char *model = argv[1];
    const char *folder = argv[2];
    size_t rows = (size_t)atoi(argv[3]);
    size_t cols = (size_t)atoi(argv[4]);

    Grid *g = grid_rebuild_from_folder_with_model(folder, rows, cols, model);
    if (!g)
    {
        fprintf(stderr, "grid_rebuild failed\n");
        return 2;
    }

    grid_print(g);

    grid_free(g);
    return 0;
}
