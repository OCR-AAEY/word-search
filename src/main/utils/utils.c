#include "utils/utils.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

int execute_command(const char *cmd)
{
    if (cmd == NULL)
    {
        printf("Command is NULL\n");
        return EXIT_FAILURE;
    }

    int cmd_return = system(cmd);

    if (cmd_return == -1)
    {
        errx(EXIT_FAILURE, "Command %s failed", cmd);
    }

    // Optionally, you can check the exit status:
    if (WIFEXITED(cmd_return))
    {
        int status = WEXITSTATUS(cmd_return);
        // printf("Command exited with code %d\n", status);
        return status;
    }
    else
    {
        errx(EXIT_FAILURE, "Command %s failed", cmd);
    }
}