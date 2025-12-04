#include "location/split_letters.h"

unsigned int get_average_letter_width(BoundingBox ***letters_boxes, size_t nb_words, size_t *word_nb_letters)
{
    unsigned int total_letters = 0;
    unsigned int sum = 0;
    for (size_t word = 0; word < nb_words; word++)
    {
        for (size_t letter = 0; letter < word_nb_letters[word]; letter++)
        {
            sum += letters_boxes[word][letter]->br.x - letters_boxes[word][letter]->tl.x;
            total_letters++;
        }
    }
    return sum / total_letters;
}

BoundingBox **split_large_letter(BoundingBox *letter_box, size_t nb_parts)
{
    if (nb_parts <= 1)
        errx(EXIT_FAILURE, "Unable to split the letter in 1 or less parts");
    unsigned int letter_width = letter_box->br.x - letter_box->tl.x;
    int split_letter_width = (int)letter_width / (int)nb_parts;

    BoundingBox **new_letter_boxes = malloc(nb_parts * sizeof(BoundingBox *));

    for (size_t i = 0; i < nb_parts; i++)
    {
        BoundingBox *new_letter_box = malloc(sizeof(BoundingBox));
        if (new_letter_box == NULL)
            errx(EXIT_FAILURE, "Failed to allocate new_letter_box");

        new_letter_box->tl.y = letter_box->tl.y;
        new_letter_box->br.y = letter_box->br.y;

        new_letter_box->tl.x = letter_box->tl.x + split_letter_width * i;
        new_letter_box->br.x = new_letter_box->tl.x + split_letter_width;
        new_letter_boxes[i] = new_letter_box;
    }
    new_letter_boxes[nb_parts-1]->br.x = letter_box->br.x;
    return new_letter_boxes;
}

BoundingBox ***detect_split_large_letters(BoundingBox ***letters_boxes, size_t nb_words, size_t *word_nb_letters)
{
    unsigned int avg_letter_width = get_average_letter_width(letters_boxes, nb_words, word_nb_letters);
    unsigned int letter_width;
    for (size_t word = 0; word < nb_words; word++)
    {
        for (size_t letter = 0; letter < word_nb_letters[word]; letter++)
        {
            letter_width = letters_boxes[word][letter]->br.x - letters_boxes[word][letter]->tl.x;
            if (letter_width / avg_letter_width >= 2)
            {
                size_t nb_parts = (size_t)(letter_width / avg_letter_width);
                BoundingBox **split_boxes = split_large_letter(letters_boxes[word][letter], nb_parts);
                size_t new_word_size = word_nb_letters[word] + nb_parts - 1;
                BoundingBox **new_word = malloc(new_word_size * sizeof(BoundingBox *));
                for (size_t prev_old_letters_index = 0; prev_old_letters_index < letter; prev_old_letters_index++)
                {
                    new_word[prev_old_letters_index] = letters_boxes[word][prev_old_letters_index];
                }

                for (size_t new_letters_index = 0; new_letters_index < nb_parts; new_letters_index++)
                {
                    new_word[letter + new_letters_index] = split_boxes[new_letters_index];
                }

                for (size_t remaining_letters_index = letter + 1; remaining_letters_index < word_nb_letters[word]; remaining_letters_index++)
                {
                    new_word[letter + nb_parts + remaining_letters_index] = letters_boxes[word][remaining_letters_index];
                }
                free(split_boxes);
                free(letters_boxes[word]);
                letters_boxes[word] = new_word;
                word_nb_letters[word] = new_word_size;
            }
        }
    }
    return letters_boxes;
}