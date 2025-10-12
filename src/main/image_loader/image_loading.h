#ifndef IMAGE_LOADING_H

#define IMAGE_LOADING_H
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Pixel;

typedef struct {
    int width;
    int height;
    Pixel *pixels;
} ImageData;

ImageData *read_image(const char *path);
void save_pixbuf_to_png(GdkPixbuf *pixbuf, char *filename);
void free_pixels(guchar *pixels, gpointer data);
void free_image(ImageData *img);

#endif
