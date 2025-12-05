// src/main/grid_rebuild/grid_rebuild_test.c
// Full extraction -> cell export -> NN inference -> Grid rebuild -> print
// Uses LEVEL_1_IMG_1 and a 17x17 grid as requested.

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "utils/utils.h"
#include "image_loader/image_loading.h"
#include "pretreatment/pretreatment.h"
#include "rotation/rotation.h"
#include "location/location.h"
#include "location/hough_lines_legacy.h"
#include "matrix/matrix.h"
#include "ocr/neural_network.h"
#include "grid_rebuild.h"
#include "solver/grid.h"

/* Default values - you can override the model path by passing it as argv[1] */
#define DEFAULT_MODEL_PATH "assets/model.bin"
#define DEFAULT_IMAGE_PATH LEVEL_2_IMG_2
#define DEFAULT_ROWS 17
#define DEFAULT_COLS 17

/* Run the project extraction pipeline (similar to the project's main pipeline)
 * The goal is to produce extracted/grid/(r_c).png using existing project
 * functions. This follows the sequence already used in your project:
 *
 *  - cleanup_folders() / setup_folders()
 *  - load_image -> image_to_grayscale
 *  - adaptative thresholding
 *  - auto deskew (rotation)
 *  - morphological transforms (closing/opening)
 *  - Hough transform lines
 *  - extract_intersection_points
 *  - extract_grid_cells -> writes to GRID_DIR/(r_c).png
 *
 * Note: we intentionally reuse the project's functions so behaviour matches.
 */
static void run_full_extraction_pipeline(const char *input_image_path)
{
    printf("[+] Running full extraction pipeline on: %s\n", input_image_path);

    cleanup_folders();
    setup_folders();

    // 1) Load image
    ImageData *img = load_image(input_image_path);
    if (!img)
        errx(EXIT_FAILURE, "Failed to load input image: %s", input_image_path);

    // 2) Convert to grayscale matrix
    Matrix *gray = image_to_grayscale(img);
    free_image(img);
    if (!gray)
        errx(EXIT_FAILURE, "image_to_grayscale failed");

    // 3) Deskew (auto rotation) step expects threshold first in your pipeline,
    //    but we follow a robust sequence: threshold -> deskew -> transforms.
    Matrix *threshold = adaptative_gaussian_thresholding(gray, 255, 7, 3, 2);
    mat_free(gray);
    if (!threshold)
        errx(EXIT_FAILURE, "Thresholding failed");

    Matrix *rotated = auto_deskew_matrix(threshold);
    mat_free(threshold);
    if (!rotated)
        errx(EXIT_FAILURE, "auto_deskew_matrix failed");

    // 4) Morphological filtering: closing then opening (same sequence used in project)
    Matrix *closing = morph_transform(rotated, 1, Closing);
    if (!closing) { mat_free(rotated); errx(EXIT_FAILURE, "morph_transform(Closing) failed"); }

    Matrix *opening = morph_transform(closing, 2, Opening);
    mat_free(closing);
    if (!opening) { mat_free(rotated); errx(EXIT_FAILURE, "morph_transform(Opening) failed"); }

    // 5) Hough transform to get lines
    size_t nb_lines = 0;
    Line **lines = hough_transform_lines(opening, 90, 5, 1, &nb_lines);
    if (!lines || nb_lines == 0) {
        mat_free(opening);
        errx(EXIT_FAILURE, "hough_transform_lines returned no lines");
    }

    // 6) Extract intersection points from Hough lines
    size_t height_points = 0, width_points = 0;
    Point **points = extract_intersection_points(lines, nb_lines, &height_points, &width_points);
    if (!points || height_points == 0 || width_points == 0) {
        free_lines(lines, nb_lines);
        mat_free(opening);
        errx(EXIT_FAILURE, "extract_intersection_points failed to detect grid intersections");
    }
    printf("[+] Found %zu x %zu intersection points\n", height_points, width_points);

    // 7) Extract grid cells -> this writes files to GRID_DIR (EXTRACT_DIR "/grid")
    extract_grid_cells(opening, points, height_points, width_points);

    // cleanup temporary structures
    for (size_t i = 0; i < height_points; ++i) free(points[i]);
    free(points);
    free_lines(lines, nb_lines);
    mat_free(opening);

    printf("[+] Extraction pipeline finished. Cells written to: %s\n", GRID_DIR);
}

/* main: optionally accept model path as argv[1]; otherwise use DEFAULT_MODEL_PATH */
int main(int argc, char **argv)
{
    const char *model_path = (argc >= 2) ? argv[1] : DEFAULT_MODEL_PATH;
    const char *image_path = DEFAULT_IMAGE_PATH;
    const size_t rows = DEFAULT_ROWS;
    const size_t cols = DEFAULT_COLS;

    printf("=== GRID REBUILD TEST (Level1 Image1 -> %zux%zu grid) ===\n", rows, cols);
    printf("[i] model: %s\n", model_path);
    printf("[i] image: %s\n", image_path);

    // 1) Run full extraction pipeline (produces extracted/grid/(r_c).png)
    run_full_extraction_pipeline(image_path);

    // 2) Rebuild grid from extracted cells using the given model
    Grid *g = grid_rebuild_from_folder_with_model(GRID_DIR, rows, cols, model_path);
    if (!g) {
        fprintf(stderr, "grid_rebuild_from_folder_with_model failed\n");
        return EXIT_FAILURE;
    }

    // 3) Print result and free
    printf("\n=== REBUILT GRID ===\n");
    grid_print(g);

    grid_free(g);

    printf("\n[OK] grid_rebuild_test finished.\n");
    return EXIT_SUCCESS;
}
