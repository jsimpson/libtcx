#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tcx.h"
#include "tcx_utils.h"

int
main(int argc, char const * argv[])
{
    tcx_t * tcx = calloc(1, sizeof(tcx_t));

    if (argc < 2)
    {
        fprintf(stderr, "Please supply a .TCX file.\n");
        free(tcx);
        return 1;
    }
    else
    {
        tcx->filename = strdup(argv[1]);
    }

    if (access(tcx->filename, F_OK) != 0)
    {
        fprintf(stderr, "Unable to locate %s.\n", tcx->filename);
        free(tcx->filename);
        free(tcx);
        return 1;
    }

    if (parse_tcx_file(tcx) == 0)
    {
        calculate_summary(tcx);
        print_activity(tcx->activities);
    }

    free(tcx->filename);
    free(tcx);
}
