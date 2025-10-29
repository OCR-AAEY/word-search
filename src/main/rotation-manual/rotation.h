#ifndef ROTATION_H
#define ROTATION_H

#include "../image_loader/image_loading.h"

// Rotate an ImageData by the given angle (in degrees)
Matrix *rotate_matrix(const Matrix *src, double angle);
ImageData *rotate_image(ImageData *img, double angle);
#endif
