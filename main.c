#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tcx.h"

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

    tcx_t * tcx = calloc(1, sizeof(tcx_t));
    if (parse_tcx_file(tcx, filename) == 0)
    {
        calculate_summary(tcx);
    }

    free(tcx);
    free(filename);
}
