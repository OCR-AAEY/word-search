#include "image_loading.h"

void save_pixbuf_to_png(GdkPixbuf *pixbuf, char *filename) {
    GError *error = NULL;
    char success = gdk_pixbuf_save(pixbuf, filename, "png", &error, NULL);
    if (!success) {
        g_printerr("Error saving image: %s\n", error->message);
        g_error_free(error);
    }
}

void free_pixels(guchar *pixels, gpointer data) {
    free(data); // data will be your ImageData->pixels pointer
}

GdkPixbuf *create_pixbuf_from_image_data(ImageData *img) {
    int width = img->width;
    int height = img->height;
    int channels = 3;  // RGB
    int rowstride = width * channels;

    // Allocate tight buffer: RGBRGBRGB...
    guchar *pixels = malloc(height * rowstride);
    if (pixels == NULL) {
        g_printerr("Failed to allocate pixel buffer for pixbuf\n");
        return NULL;
    }

    for (size_t i = 0; i < height * width; i++)
    {
        pixels[3*i] = img->pixels[i].r;
        pixels[3*i+1] = img->pixels[i].g;
        pixels[3*i+2] = img->pixels[i].b;
    }

    // Create pixbuf from raw data. The last param is the user data passed to free_pixels.
    return gdk_pixbuf_new_from_data(
        pixels,
        GDK_COLORSPACE_RGB,
        FALSE,         // has_alpha = FALSE
        8,             // bits per sample (8 bits per color RGB)
        width,
        height,
        rowstride,     // number of bytes per row
        free_pixels,   // callback to free pixel function
        pixels    // data to pass to free_pixels
    );
}

ImageData *read_image(const char *path) {
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(path, &error);

    if (!pixbuf) {
        g_printerr("Error loading image: %s\n", error->message);
        g_error_free(error);
        return NULL;
    }

    int w = gdk_pixbuf_get_width(pixbuf);
    int h = gdk_pixbuf_get_height(pixbuf);
    int channels = gdk_pixbuf_get_n_channels(pixbuf);
    if (channels < 3)
    {
        g_object_unref(pixbuf);
        g_printerr("Unsupported channel number in image loaded");
        return NULL;
    }

    unsigned char *pixels = gdk_pixbuf_get_pixels(pixbuf);

    // Allocate an array of h*w Pixels
    Pixel *pixels_copy = malloc(h * w * sizeof(Pixel));
    if (pixels_copy == NULL)
    {
        g_object_unref(pixbuf);
        g_printerr("Failed to allocate pixels copy");
        return NULL;
    }

    for (size_t i = 0; i < h  *w; i++) {
        pixels_copy[i].r = pixels[i*channels];
        pixels_copy[i].g = pixels[i*channels + 1];
        pixels_copy[i].b = pixels[i*channels + 2];
    }
    g_object_unref(pixbuf);

    ImageData *img = malloc(sizeof(ImageData));
    if (!img) {
        free(pixels_copy);
        g_printerr("Failed to allocate ImageData struct");
        return NULL;
    }

    img->width = w;
    img->height = h;
    img->pixels = pixels_copy;

    return img;
}

void free_image(ImageData *img) {
    free(img->pixels);
    free(img);
}

#ifndef UNIT_TEST

int main(int argc, char **argv) {
    ImageData *img = read_image("assets/sample_images/level_1_image_1.png");
    GdkPixbuf* pixbuf = create_pixbuf_from_image_data(img);
    save_pixbuf_to_png(pixbuf, "image1.png");
    g_object_unref(pixbuf);
    free_image(img);

    return EXIT_SUCCESS;
}

#endif