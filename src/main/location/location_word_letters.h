#ifndef LOCATION_WORDS_H
#define LOCATION_WORDS_H
#include "location/location.h"

#undef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/// @brief Finds the largest remaining area outside a given bounding box within
/// the source image.
/// @param[in] grid_box Pointer to the BoundingBox representing the current
/// area. Must not be NULL.
/// @param[in] src_height Height of the source image.
/// @param[in] src_width Width of the source image.
/// @return Pointer to a newly allocated BoundingBox representing the largest
/// remaining area,
///         or NULL if the bounding box exceeds source boundaries or no
///         remaining space exists.
BoundingBox *find_biggest_remaining_area(BoundingBox *grid_box,
                                         size_t src_height, size_t src_width);

/// @brief Extracts word regions from a source matrix and saves each as a PNG.
/// Each word is saved as WORD_BASE_DIR/<index>/full_word.png.
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] words_boxes Array of pointers to word bounding boxes. Must not be
/// NULL; each box must not be NULL.
/// @param[in] nb_words Number of words/bounding boxes in the array.
/// @return 0 on success, or a negative error code on failure:
///         -1 if src is NULL,
///         -2 if words_boxes is NULL,
///         -3 if any word box is NULL,
///         -4 if saving any word fails.
int extract_words(Matrix *src, BoundingBox **words_boxes, size_t nb_words);

/// @brief Extracts letter regions from a source matrix and saves each as a PNG.
/// @param src Source matrix (must not be NULL).
/// @param letter_boxes 2D array of pointers to letter bounding boxes per word
///        (must not be NULL; each inner array must not be NULL).
/// @param nb_words Number of words.
/// @param words_nb_letters Array giving the number of letters for each word
///        (must not be NULL).
/// @return 0 on success, or a negative status code on error (invalid input,
///         missing letter boxes, or failure to save a letter PNG).
/// @note Saves each letter as
///       WORD_BASE_DIR/<word_index>/LETTERS_DIR/<letter_index>.png.
int extract_letters(Matrix *src, BoundingBox ***letter_boxes, size_t nb_words,
                    size_t *words_nb_letters);

/// @brief Saves the region defined by a bounding box from a source matrix as a
/// PNG.
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] box Pointer to the bounding box defining the region to extract.
/// Must not be NULL.
/// @param[in] filename Path to the output PNG file.
/// @return 0 on success, or EXIT_FAILURE if the extraction or saving fails.
int extract_boundingbox_to_png(Matrix *src, BoundingBox *box,
                               const char *filename);

/// @brief Computes bounding boxes of letters inside each word region.
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] words_boxes Array of word bounding boxes. Must not be NULL.
/// @param[in] nb_words Number of words in the array.
/// @param[in] threshold Threshold used for letter segmentation.
/// @param[out] size_out Pointer to an array that will store the number of
/// letters found per word. Must not be NULL; memory is allocated inside the
/// function.
/// @return A 2D array of allocated BoundingBox pointers for each word,
///         or NULL on error (e.g., invalid parameters or memory allocation
///         failure).
BoundingBox ***get_bounding_box_letters(Matrix *src, BoundingBox **words_boxes,
                                        size_t nb_words, size_t threshold,
                                        size_t **size_out);

/// @brief Finds letter bounding boxes within a word using a vertical histogram
/// with threshold.
/// @param area Bounding box of the word region.
/// @param histogram Vertical histogram array (must not be NULL).
/// @param size Histogram size.
/// @param threshold Minimum value for detecting letter regions.
/// @param size_out Output parameter storing the number of detected letters
///        (must not be NULL).
/// @return An array of allocated BoundingBox pointers representing detected
/// letters or NULL on error.
BoundingBox **find_letters_histogram_threshold(BoundingBox *area,
                                               size_t *histogram, size_t size,
                                               size_t threshold,
                                               size_t *size_out);

/// @brief Computes a vertical histogram of black pixels within a given area.
/// @param src Source matrix (must not be NULL).
/// @param area Bounding box defining the region to analyze (must not be NULL).
/// @param size_out Output parameter storing the histogram width (must not be
/// NULL).
/// @return A newly allocated array where each element counts black pixels in a
/// column or NULL in case of error.
size_t *histogram_vertical(Matrix *src, BoundingBox *area, size_t *size_out);

/// @brief Detects and returns bounding boxes of words within a specified area.
/// @param[in] src Pointer to the source matrix. Must not be NULL.
/// @param[in] area Pointer to the bounding box defining the region to analyze.
/// Must not be NULL.
/// @param[in] threshold Threshold used for word segmentation.
/// @param[in] area_padding Padding applied to the input area before processing.
/// @param[in] word_margin Extra margin added to each detected word box.
/// @param[out] size_out Pointer to store the number of detected words. Must not
/// be NULL.
/// @return Array of allocated BoundingBox pointers representing detected words,
///         or NULL on error (e.g., invalid parameters or memory allocation
///         failure).
BoundingBox **get_bounding_box_words(Matrix *src, BoundingBox *area,
                                     size_t threshold, size_t area_padding,
                                     size_t word_margin, size_t *size_out);

/// @brief Finds word bounding boxes within an area using a horizontal histogram
/// thresholded.
/// @param area Bounding box of the region to analyze.
/// @param histogram Horizontal histogram array (must not be NULL).
/// @param size Histogram size.
/// @param threshold Minimum histogram value required to detect a word region.
/// @param size_out Output parameter storing the number of detected words
///        (must not be NULL).
/// @return An array of allocated BoundingBox pointers representing detected
/// words.
BoundingBox **find_words_histogram_threshold(BoundingBox *area,
                                             size_t *histogram, size_t size,
                                             size_t threshold,
                                             size_t *size_out);

/// @brief Expands a bounding box by adding margins on each side.
/// @param box Bounding box to modify (must not be NULL).
/// @param top Margin added to the top.
/// @param bottom Margin added to the bottom.
/// @param right Margin added to the right.
/// @param left Margin added to the left.
void margin_bounding_box(BoundingBox *box, size_t top, size_t bottom,
                         size_t right, size_t left);

/// @brief Shrinks a bounding box by applying padding from each side.
/// @param[in,out] box Pointer to the BoundingBox to modify. Must not be NULL.
/// @param[in] top Padding to remove from the top.
/// @param[in] bottom Padding to remove from the bottom.
/// @param[in] right Padding to remove from the right.
/// @param[in] left Padding to remove from the left.
/// @return 0 on success, or -1 if the padding exceeds the box size.
int pad_bounding_box(BoundingBox *box, size_t top, size_t bottom, size_t right,
                     size_t left);

/// @brief Computes a horizontal histogram of black pixels within a given area.
/// @param src Source matrix (must not be NULL).
/// @param area Bounding box defining the region to analyze (must not be NULL).
/// @param size_out Output parameter storing the histogram height (must not be
/// NULL).
/// @return A newly allocated array where each element counts black pixels in a
/// row.
size_t *histogram_horizontal(Matrix *src, BoundingBox *area, size_t *size_out);

#endif