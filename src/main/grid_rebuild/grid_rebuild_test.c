#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "grid_rebuild.h"

#include "ocr/neural_network.h"
/* Image & preprocessing */
#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "pretreatment/pretreatment.h"

/* Grid detection */
#include "location/location.h"
#include "location/hough_lines_legacy.h"

#include "rotation/rotation.h"

/* Utils */
#include "utils/utils.h"

/* Solver (for Grid printing) */
#include "solver/grid.h"

/* Sample image */
#define TEST_IMAGE LEVEL_1_IMG_1
#define GRID_ROWS 17
#define GRID_COLS 17
#define MODEL_PATH "assets/model.bin"
#define GRID_FOLDER GRID_DIR

static void run_full_extraction_pipeline(void)
{
    /* Cleanup and prepare folders */
    cleanup_folders();
    setup_folders();

    /* Load image */
    ImageData *img = load_image(TEST_IMAGE);
    if (!img)
        errx(EXIT_FAILURE, "Failed to load test image");

    Matrix *m = image_to_grayscale(img);
    free_image(img);

    /* Pretreatment pipeline */
    Matrix *tmp;

    tmp = auto_deskew_matrix(m);
    mat_free(m);
    m = tmp;

    tmp = adaptative_gaussian_thresholding(m, 255, 11, 10, 5);
    mat_free(m);
    m = tmp;

    tmp = morph_transform(m, 2, Closing);
    mat_free(m);
    m = tmp;

    tmp = morph_transform(m, 2, Opening);
    mat_free(m);
    m = tmp;

    /* Hough lines */
    size_t nb_lines;
    Line **lines = hough_transform_lines(m, 90, 5, 1, &nb_lines);

    if (!lines)
        errx(EXIT_FAILURE, "Hough transform failed");

    /* Intersection points */
    size_t h_points, w_points;
    Point **points = extract_intersection_points(
        lines, nb_lines, &h_points, &w_points);

    if (!points)
        errx(EXIT_FAILURE, "Intersection extraction failed");

    /* Extract grid cells */
    extract_grid_cells(m, points, h_points, w_points);

    /* Cleanup */
    free_points(points, h_points);
    free_lines(lines, nb_lines);
    mat_free(m);
}

int main(void)
{
    printf("=== GRID REBUILD TEST ===\n");

    run_full_extraction_pipeline();

    printf("\n--- Rebuilding grid with OCR ---\n\n");

    Grid *g = grid_rebuild_from_folder_with_model(
        GRID_FOLDER, GRID_ROWS, GRID_COLS, MODEL_PATH);

    if (!g)
        errx(EXIT_FAILURE, "Grid rebuild failed");

    grid_print(g);
    grid_free(g);

    printf("\n=== DONE ===\n");
    return EXIT_SUCCESS;
}
