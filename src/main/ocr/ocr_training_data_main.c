// #include <dirent.h>
// #include <err.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <sys/types.h>

// #include "image_loader/image_loading.h"
// #include "matrix/matrix.h"

// void process_file(const char *filepath, const char *filename)
// {
//     ImageData *img = load_image(filepath);
//     Matrix *m = mat_create_zero(img->height, img->width);
//     for (size_t h = 0; h < mat_height(m); h++)
//     {
//         for (size_t w = 0; w < mat_width(m); w++)
//         {
//             Pixel p = img->pixels[h * img->width + w];
//             if (p.r > 127 || p.g > 127 || p.b > 127)
//                 *mat_coef_ptr(m, h, w) = 0.0f;
//             else
//                 *mat_coef_ptr(m, h, w) = 1.0f;
//         }
//     }

//     m = mat_strip_margins(m);
//     m = mat_scale_to_28(m);

//     char newpath[1024];
//     snprintf(newpath, sizeof(newpath), "./assets/ocr-training-dataset/matrices/%s",
//              filename);

//     mat_save_to_file(m, newpath);
// }

// int main(void)
// {
//     char *dirname = "./assets/ocr-training-dataset/cropped/";

//     DIR *dir = opendir(dirname);
//     if (dir == NULL)
//     {
//         perror("opendir");
//         return 1;
//     }

//     struct dirent *entry;
//     char path[1024];

//     while ((entry = readdir(dir)) != NULL)
//     {
//         // Skip "." and ".."
//         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
//             continue;

//         snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

//         struct stat st;
//         if (stat(path, &st) == -1)
//         {
//             perror("stat");
//             continue;
//         }

//         if (S_ISREG(st.st_mode))
//         { // Only process regular files
//             process_file(path, entry->d_name);
//         }
//     }

//     closedir(dir);
// }
