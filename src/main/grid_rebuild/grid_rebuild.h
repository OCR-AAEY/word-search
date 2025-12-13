#ifndef GRID_REBUILD_H
#define GRID_REBUILD_H

#include <stddef.h>

/*
 * Forward declaration.
 * The real definition is in solver/grid.h
 */
typedef struct Grid Grid;

/*
 * Rebuilds the grid by recognizing each cell image with an OCR neural network.
 *
 * folder:
 *   Path to extracted grid cells (ex: "extracted/grid")
 *
 * rows / cols:
 *   Grid dimensions (ex: 17x17)
 *
 * model_path:
 *   Path to the OCR neural network model
 *
 * Returns:
 *   A newly allocated Grid* (must be freed with grid_free),
 *   or NULL on error.
 */
Grid *grid_rebuild_from_folder_with_model(const char *folder, size_t rows,
                                          size_t cols, const char *model_path);

#endif /* GRID_REBUILD_H */
