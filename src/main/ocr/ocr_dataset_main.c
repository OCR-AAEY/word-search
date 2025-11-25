#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "image_loader/image_loading.h"
#include "matrix/matrix.h"

/*
int main(void)
{

    double angles[3] = {0.0, 0.0, 25.0};
    char *filenames[3] = {"assets/sample_images/level_1_image_1.png",
                          "assets/sample_images/level_1_image_2.png",
                          "assets/sample_images/level_2_image_1.png"};
    int levels[3] = {1, 1, 2};
    int images[3] = {1, 2, 1};

    for (size_t i = 0; i < 3; i++)
    {
        cleanup_folders();
        setup_folders();

        Matrix *res, *tmp;
        ImageData *img = load_image(filenames[i]);
        res = image_to_grayscale(img);
        free_image(img);

        if (angles[i] != 0.0f)
        {
            tmp = rotate_matrix(res, angles[i]);
            free(res);
            res = tmp;
        }

        tmp = adaptative_gaussian_thresholding(res, 255, 11, 10, 5);
        mat_free(res);
        res = tmp;

        tmp = morph_transform(res, 2, Closing);
        free(res);
        res = tmp;

        tmp = morph_transform(res, 2, Opening);
        free(res);
        res = tmp;

        size_t nb_lines;
        Line **lines = hough_transform_lines(res, 90, 5, 1, &nb_lines);

        size_t width_points, height_points;
        Point **points = extract_intersection_points(
            lines, nb_lines, &height_points, &width_points);

        extract_grid_cells(res, points, height_points, width_points);

        BoundingBox *grid_box =
            get_bounding_box_grid(points, height_points, width_points);
        free_points(points, height_points);

        BoundingBox *remaining_box = find_biggest_remaining_area(
            grid_box, mat_height(res), mat_width(res));



        size_t nb_words;
        BoundingBox **words_boxes =
            get_bounding_box_words(res, remaining_box, 5, 20, 4, &nb_words);

        free(remaining_box);

        extract_words(res, words_boxes, nb_words);

        size_t *word_nb_letters;
        BoundingBox ***letters_boxes = get_bounding_box_letters(
            res, words_boxes, nb_words, 0, &word_nb_letters);

        extract_letters(res, letters_boxes, nb_words, word_nb_letters);

        for (size_t i = 0; i < nb_words; i++)
        {
            free_bboxes(letters_boxes[i], word_nb_letters[i]);
        }
        free(letters_boxes);
        free(word_nb_letters);
        free_bboxes(words_boxes, nb_words);
        free(grid_box);
        mat_free(res);
        free_lines(lines, nb_lines);

        for ()rename()
    }

    return EXIT_SUCCESS;
}
*/

/*
void process_file(const char *filepath, const char *filename)
{
    ImageData *img = load_image(filepath);
    Matrix *m = mat_create_zero(img->height, img->width);
    for (size_t h = 0; h < mat_height(m); h++)
    {
        for (size_t w = 0; w < mat_width(m); w++)
        {
            Pixel p = img->pixels[h * img->width + w];
            if (p.r > 127 || p.g > 127 || p.b > 127)
                *mat_coef_ptr(m, h, w) = 0.0f;
            else
                *mat_coef_ptr(m, h, w) = 1.0f;
        }
    }

    m = mat_strip_margins(m);
    m = mat_scale_to_28(m);
    mat_inplace_to_one_hot(m);

    char newpath[1024];
    snprintf(newpath, sizeof(newpath),
             "./assets/ocr-training-dataset/matrices/%s", filename);

    mat_save_to_file(m, newpath);
}

int main(void)
{
    char *dirname = "./assets/ocr-training-dataset/cropped/";

    DIR *dir = opendir(dirname);
    if (dir == NULL)
    {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    char path[1024];

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        struct stat st;
        if (stat(path, &st) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISREG(st.st_mode))
        { // Only process regular files
            process_file(path, entry->d_name);
        }
    }

    closedir(dir);
}
*/
