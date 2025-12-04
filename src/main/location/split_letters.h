#ifndef LOCATION_SPLIT_LETTERS
#define LOCATION_SPLIT_LETTERS

#include "location/location.h"

// TODO : BFS to make connectivity checks on letters and split if not connected
// TODO : Average size of letters and if letter > 2*avg, get the number of avg letters is has and divide by this number the box - Check

unsigned int get_average_letter_width(BoundingBox ***letters_boxes, size_t nb_words, size_t *word_nb_letters);
BoundingBox **split_large_letter(BoundingBox *letter_box, size_t nb_parts);
BoundingBox ***detect_split_large_letters(BoundingBox ***letters_boxes, size_t nb_words, size_t *word_nb_letters);

#endif