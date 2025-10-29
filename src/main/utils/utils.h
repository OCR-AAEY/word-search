#ifndef UTILS_H
#define UTILS_H

// DIR CONSTANTS
#define EXTRACT_DIR "extracted"
#define WORDS_DIR EXTRACT_DIR "/words"
#define WORD_BASE_DIR(a) "word_" #a
#define GRID_DIR EXTRACT_DIR "/grid"
#define EXAMPLES_DIR EXTRACT_DIR "/examples"

// OUTPUT FILES CONSTANTS
#define POSTTREATMENT_FILENAME EXAMPLES_DIR "/treated_image.png"
#define HOUGHLINES_VISUALIZATION_FILENAME EXAMPLES_DIR "/hough_lines.png"
#define INTERSECTION_POINTS_FILENAME EXAMPLES_DIR "/intersection_points.png"
#define BOUNDING_BOXES_FILENAME EXAMPLES_DIR "/bounding_boxes.png"

// SAMPLE IMAGES PATH
#define LEVEL_1_IMG_1 "assets/sample_images/level_1_image_1.png"
#define LEVEL_1_IMG_2 "assets/sample_images/level_1_image_2.png"
#define LEVEL_2_IMG_1 "assets/sample_images/level_2_image_1.png"
#define LEVEL_2_IMG_2 "assets/sample_images/level_2_image_2.png"
#define LEVEL_3_IMG_1 "assets/sample_images/level_3_image_1.png"
#define LEVEL_3_IMG_2 "assets/sample_images/level_3_image_2.png"

int execute_command(const char *cmd);

#endif