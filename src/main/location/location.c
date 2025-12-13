#include "location/location.h"
#include "extract_char/extract_char.h"
#include "rotation/rotation.h"
#include "utils/utils.h"
#include <stdio.h>
#include <string.h>

void free_bboxes(BoundingBox **boxes, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        free(boxes[i]);
    }
    free(boxes);
}

void cleanup_folders()
{
    char cmd[255];
    sprintf(cmd, "rm -rf %s", EXTRACT_DIR);
    execute_command(cmd);
}

void setup_folders()
{
    char cmd[255];
    int status;
    sprintf(cmd, "mkdir %s", EXTRACT_DIR);
    status = execute_command(cmd);
    if (status == EXIT_FAILURE)
        errx(EXIT_FAILURE, "Command create extract dir failed");
    sprintf(cmd, "mkdir %s", WORDS_DIR);
    status = execute_command(cmd);
    if (status == EXIT_FAILURE)
        errx(EXIT_FAILURE, "Command create words dir failed");
    sprintf(cmd, "mkdir %s", GRID_DIR);
    status = execute_command(cmd);
    if (status == EXIT_FAILURE)
        errx(EXIT_FAILURE, "Command create grid dir failed");
    sprintf(cmd, "mkdir %s", EXAMPLES_DIR);
    status = execute_command(cmd);
    if (status == EXIT_FAILURE)
        errx(EXIT_FAILURE, "Command create examples failed failed");
}

void setup_words_folders(size_t nb_words)
{
    char cmd[255];
    int status;
    for (size_t i = 0; i < nb_words; i++)
    {
        sprintf(cmd, "mkdir %s%zu", WORD_BASE_DIR, i);
        status = execute_command(cmd);
        if (status == EXIT_FAILURE)
            errx(EXIT_FAILURE, "Command create %s%zu dir failed", WORD_BASE_DIR,
                 i);

        sprintf(cmd, "mkdir %s%zu/%s", WORD_BASE_DIR, i, LETTERS_DIR);
        status = execute_command(cmd);
        if (status == EXIT_FAILURE)
            errx(EXIT_FAILURE, "Command create %s%zu/%s dir failed",
                 WORD_BASE_DIR, i, LETTERS_DIR);
    }
}
