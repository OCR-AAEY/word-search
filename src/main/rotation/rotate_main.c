#include "image_loader/image_loading.h"
#include "matrix/matrix.h"
#include "pretreatment/pretreatment.h"
#include "pretreatment/visualization.h"
#include "rotation/hough_lines.h"
#include "rotation/rotation.h"
#include "utils/utils.h"

int main()
{
    ImageData *img = load_image(LEVEL_2_IMG_1);
    Matrix *gray = image_to_grayscale(img);
    Matrix *threshold = adaptative_gaussian_thresholding(gray, 255, 11, 10, 5);
    Matrix *rotated = auto_rotate_matrix(threshold);
    export_matrix(rotated, ROTATED_FILENAME);
    free_image(img);
    mat_free(threshold);
    mat_free(gray);
    mat_free(rotated);
}