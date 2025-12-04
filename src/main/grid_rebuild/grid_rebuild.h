#ifndef GRID_REBUILD_H
#define GRID_REBUILD_H

#include "neural_network/neural_network.h"
#include "solver/grid.h"
#include <stdlib.h>

/*
 * Recognize a single letter from a PNG file by using the given neural network.
 * - path: path to the PNG file (e.g. "grid_rebuild/(0_0).png")
 * - net: a loaded Neural_Network* (must be created with net_load_from_file)
 *
 * Returns an uppercase letter 'A'..'Z' on success, or '?' on failure.
 */
char recognize_letter_from_png(const char *path, Neural_Network *net);

/*
 * Rebuild a Grid from PNGs stored in folder.
 * - folder: folder containing files named "(row_col).png" (e.g.
 * folder/(0_0).png)
 * - rows, cols: grid dimensions
 * - model_path: path to a trained neural network file usable by
 * net_load_from_file
 *
 * This function loads the model from model_path, then reads each cell PNG,
 * recognizes it, and constructs a Grid* whose content is row-major char array.
 *
 * Returns a newly-allocated Grid* (use grid_free(Grid*) to free), or NULL on
 * error.
 */
Grid *grid_rebuild_from_folder_with_model(const char *folder, size_t rows,
                                          size_t cols, const char *model_path);

#endif /* GRID_REBUILD_H */
