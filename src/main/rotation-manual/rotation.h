#ifndef ROTATION_H
#define ROTATION_H

#include "../image_loader/image_loading.h"

// Rotate an ImageData by the given angle (in degrees)
ImageData *rotate_image(ImageData *src, double angle);

#endif
