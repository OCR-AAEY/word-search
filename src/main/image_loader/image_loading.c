#include "image_loading.h"
#include <err.h>
#include <stdio.h>

/// @brief Saves the Pixbuf object as a PNG file.
/// @param[in] pixbuf A pointer to the GdkPixbuf to save.
/// @param[in] filename The string filename with extension (.png).
/// @param[out] error Return location for a GError.
///  If not NULL and an error occurs, the error will be set.
///  The caller is responsible for freeing it with g_error_free().
/// @return A boolean indicating if the file was successfully saved.
int save_pixbuf_to_png(GdkPixbuf *pixbuf, char *filename, GError **error)
{
    if (error == NULL)
        errx(EXIT_FAILURE, "You must provide a GError to save the pixbuf");
    // Extract directory path
    char *dir = g_path_get_dirname(filename);

    // Create directory (recursively if needed)
    g_mkdir_with_parents(dir, 0755);
    g_free(dir);

    int success = gdk_pixbuf_save(pixbuf, filename, "png", error, NULL);
    if (!success)
    {
        g_printerr("Error saving image: %s\n", (*error)->message);
    }
    return success;
}

/// @brief Callback function used when GdkPixbuf object created by
/// `create_pixbuf_from_image_data` is destroyed.
/// @param[in] pixels A pointer to the raw pixel data (not used in this
/// function).
/// @param[in] data A pointer to the memory to be freed. This should match the
///  `pixels` pointer passed during creation of the GdkPixbuf.
void free_pixels(guchar *pixels, gpointer data)
{
    (void)pixels;
    free(data); // free pixels from gdkpixbuf object when it has to be deleted
}

/// @brief Creates a GdkPixbuf from an ImageData struct.
/// @param[in] img The ImageData to transform into a GdkPixbuf.
/// @returns The GdkPixbuf containing the same pixel data as the ImageData
/// passed as parameter or NULL if the pixel allocation failed.
GdkPixbuf *create_pixbuf_from_image_data(ImageData *img)
{
    int width = img->width;
    int height = img->height;
    int channels = 3; // RGB
    int rowstride = width * channels;

    guchar *pixels = malloc(height * rowstride);
    if (pixels == NULL)
    {
        fprintf(stderr, "Failed to allocate the pxiel array");
        return NULL;
    }

    for (int i = 0; i < height * width; i++)
    {
        pixels[3 * i] = img->pixels[i].r;
        pixels[3 * i + 1] = img->pixels[i].g;
        pixels[3 * i + 2] = img->pixels[i].b;
    }

    // Create pixbuf from raw data. The last param is the pixel data passed to
    // free_pixels. Since it is allocated by the user, it has to be freed by a
    // user made function
    return gdk_pixbuf_new_from_data(
        pixels, GDK_COLORSPACE_RGB,
        FALSE, // has_alpha = FALSE
        8,     // bits per sample (8 bits per color RGB)
        width, height,
        rowstride,   // number of bytes per row
        free_pixels, // callback to free_pixel function
        pixels       // data to pass to free_pixels
    );
}

/// @brief Loads an image file.
/// @attention This function keeps only the 3 first channels of the image
/// (normally RGB).
/// @param[in] filename The filename of the image to load.
/// @returns An ImageData containing the data of the loaded image or NULL in
/// case of error.
ImageData *load_image(const char *filename)
{
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, &error);

    if (pixbuf == NULL)
    {
        fprintf(stderr, "Error loading image: %s\n", error->message);
        g_error_free(error);
        return NULL;
    }

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int channels = gdk_pixbuf_get_n_channels(pixbuf);
    if (channels < 3)
    {
        g_object_unref(pixbuf);
        fprintf(stderr, "Unsupported channel number in the image loaded");
        return NULL;
    }

    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    // Allocate an array of h*w Pixels for the ImageData
    Pixel *pixels_copy = malloc(height * width * sizeof(Pixel));
    if (pixels_copy == NULL)
    {
        g_object_unref(pixbuf);
        fprintf(stderr, "Failed to allocate pixels copy");
        return NULL;
    }

    // Populates the Pixel data

    for (int h = 0; h < height; h++)
    {
        guchar *row = pixels + h * rowstride;
        for (int w = 0; w < width; w++)
        {
            pixels_copy[h * width + w].r = row[w * channels];
            pixels_copy[h * width + w].g = row[w * channels + 1];
            pixels_copy[h * width + w].b = row[w * channels + 2];
        }
    }
    g_object_unref(pixbuf);

    ImageData *img = malloc(sizeof(ImageData));
    if (img == NULL)
    {
        free(pixels_copy);
        fprintf(stderr, "Failed to allocate ImageData struct");
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->pixels = pixels_copy;

    return img;
}

/// @brief Frees the ImageData allocated on the heap.
/// @attention It will free the pixels array and the ImageData itself.
/// @param[in] img The ImageData to free.
void free_image(ImageData *img)
{
    free(img->pixels);
    free(img);
}

Pixel *get_pixel(ImageData *img, size_t h, size_t w)
{
    if (h >= img->height || w >= img->width)
        return NULL;

    return &(img->pixels[h * img->width + w]);
}

// #ifndef UNIT_TEST

// int main()
// {
//     ImageData *img = load_image("assets/sample_images/level_1_image_1.png");
//     GdkPixbuf *pixbuf = create_pixbuf_from_image_data(img);
//     save_pixbuf_to_png(pixbuf, "image1.png", NULL);
//     g_object_unref(pixbuf);
//     free_image(img);

//     return EXIT_SUCCESS;
// }

// #endif