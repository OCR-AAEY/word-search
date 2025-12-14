#ifndef GRID_REBUILD_H
#define GRID_REBUILD_H

#include <stddef.h>

typedef struct Grid
{
    size_t height;
    size_t width;
    char *content;
} Grid;

/// @brief Rebuild a grid from the extracted folder using the OCR model.
///        The function automatically detects the number of rows and columns
///        from the folder contents.
/// @param folder Path to the folder containing extracted grid cells.
/// @param model_path Path to the OCR model file.
/// @return Pointer to a newly allocated Grid on success, NULL on failure.
Grid *grid_rebuild_from_folder_with_model(const char *folder, const char *model_path);



#endif
