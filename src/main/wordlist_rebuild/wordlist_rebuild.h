#ifndef WORDLIST_REBUILD_H
#define WORDLIST_REBUILD_H

typedef struct
{
    int count;    // number of words
    char **words; // array of words
    int *lengths; // length of each word
} Wordlist;

/// @brief Rebuild a wordlist from a folder using the OCR model.
///        Automatically detects the number of words from folder contents.
/// @param folder Path to the folder containing word folders ("word_0",
/// "word_1", etc.)
/// @param model_path Path to the OCR model file.
/// @return Pointer to a newly allocated Wordlist on success, NULL on failure.
Wordlist *wordlist_rebuild_from_folder(const char *folder,
                                       const char *model_path);

/// @brief Free the memory of a Wordlist.
void wordlist_free(Wordlist *wl);

#endif
