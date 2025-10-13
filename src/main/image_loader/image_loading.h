#ifndef IMAGE_LOADING_H

#define IMAGE_LOADING_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <stdint.h>

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Pixel;

typedef struct
{
    int width;
    int height;
    Pixel *pixels;
} ImageData;

ImageData *load_image(const char *path);
int save_pixbuf_to_png(GdkPixbuf *pixbuf, char *filename, GError **error);
void free_pixels(guchar *pixels, gpointer data);
void free_image(ImageData *img);

#endif
