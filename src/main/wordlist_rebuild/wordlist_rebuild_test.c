#include <stdio.h>
#include "wordlist_rebuild.h"

int main(void)
{
    const char *folder = "extracted/words";
    const char *model_path = "assets/ocr/model/real.nn";

    // Rebuild wordlist; number of words detected automatically
    Wordlist *wl = wordlist_rebuild_from_folder(folder, model_path);
    if (!wl)
    {
        fprintf(stderr, "Failed to rebuild wordlist\n");
        return 1;
    }

    printf("Found %d words:\n", wl->count);
    for (int i = 0; i < wl->count; i++)
        printf("%s (length %d)\n", wl->words[i], wl->lengths[i]);

    wordlist_free(wl);
    return 0;
}
