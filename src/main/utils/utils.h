#ifndef UTILS_H
#define UTILS_H

// DIR CONSTANTS
#define EXTRACT_DIR "extracted"
#define WORDS_DIR EXTRACT_DIR "/words"
#define WORD_BASE_DIR WORDS_DIR "/word_"
#define LETTERS_DIR "/letters"
#define GRID_DIR EXTRACT_DIR "/grid"
#define EXAMPLES_DIR EXTRACT_DIR "/examples"

// OUTPUT FILES CONSTANTS
#define GRAYSCALED_FILENAME EXAMPLES_DIR "/01_grayscaled.png"
#define GAUSSIAN_BLURRED_FILENAME EXAMPLES_DIR "/02_gaussian_blurred.png"
#define THRESHOLDED_FILENAME EXAMPLES_DIR "/03_thresholded.png"
#define OPENING_FILENAME EXAMPLES_DIR "/04_opening.png"
#define CLOSING_FILENAME EXAMPLES_DIR "/05_closing.png"
#define POSTTREATMENT_FILENAME EXAMPLES_DIR "/06_post_treatment.png"
#define HOUGHLINES_VISUALIZATION_FILENAME EXAMPLES_DIR "/07_hough_lines.png"
#define INTERSECTION_POINTS_FILENAME EXAMPLES_DIR "/08_intersection_points.png"
#define GRID_BOUNDING_BOXES_FILENAME EXAMPLES_DIR "/09_grid_bounding_boxes.png"
#define WORDS_BOUNDING_BOXES_FILENAME EXAMPLES_DIR "/10_words_bounding_boxes.png"
#define LETTERS_BOUNDING_BOXES_FILENAME                                        \
    EXAMPLES_DIR "/11_letters_bounding_boxes.png"

// SAMPLE IMAGES PATH
#define LEVEL_1_IMG_1 "assets/sample_images/level_1_image_1.png"
#define LEVEL_1_IMG_2 "assets/sample_images/level_1_image_2.png"
#define LEVEL_2_IMG_1 "assets/sample_images/level_2_image_1.png"
#define LEVEL_2_IMG_2 "assets/sample_images/level_2_image_2.png"
#define LEVEL_3_IMG_1 "assets/sample_images/level_3_image_1.png"
#define LEVEL_3_IMG_2 "assets/sample_images/level_3_image_2.png"

int execute_command(const char *cmd);

#endif