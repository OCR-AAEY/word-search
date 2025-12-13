#include "matrix.h"
#include <err.h>

int main(int argc, char **argv)
{
    if (argc < 2)
        errx(EXIT_FAILURE, "Missing amtrix");
    if (argc > 2)
        errx(EXIT_FAILURE,
             "Too many parameters were given. Only 1 was expected.");

    Matrix *m = mat_load_from_file(argv[1]);

    mat_display(m);

    mat_free(m);

    return EXIT_SUCCESS;
}