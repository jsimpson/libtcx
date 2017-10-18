#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tcx.h"

char * get_file_extension(char * filename);

char *
get_file_extension(char * filename)
{
    char * ext = strrchr(filename, '.');
    if (!ext || ext == filename)
    {
        return "";
    }

    return ext + 1;
}

int
main(int argc, char const * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Please supply a TCX or GPX file.\n");
        return 1;
    }

    char * filename = strdup(argv[1]);

    if (access(filename, F_OK) != 0)
    {
        fprintf(stderr, "Unable to locate %s.\n", filename);
        free(filename);
        return 1;
    }

    char * ext = get_file_extension(filename);
    if (strncmp(ext, "tcx", 3) == 0)
    {
        tcx_t * tcx = calloc(1, sizeof(tcx_t));

        if (parse_tcx_file(tcx, filename) == 0)
        {
            calculate_summary(tcx);
        }

        free(tcx);
    }

    free(filename);
}
