#ifndef ROTATION_H
#define ROTATION_H

#include <gtk/gtk.h>

/**
 * @brief Rotates a GdkPixbuf around its center by a given angle (degrees).
 * @param src   The source GdkPixbuf.
 * @param angle The rotation angle in degrees.
 * @return A newly allocated rotated GdkPixbuf (caller must unref it).
 */
GdkPixbuf *rotate_pixbuf(GdkPixbuf *src, double angle);

#endif
