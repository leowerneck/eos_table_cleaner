#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "median_filter.h"
#include "stellar_collapse_eos.h"

int
main(int argc, char **argv)
{

    if(argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <input file> [output file]\n", argv[0]);
        exit(0);
    }

    stellar_collapse_eos *table = read_stellar_collapse_eos_table(argv[1]);

    APPLY_MEDIAN_FILTER(table, dpdrhoe);
    APPLY_MEDIAN_FILTER(table, dpderho);
    APPLY_MEDIAN_FILTER(table, dedt);

    char outfile[1024] = {0};
    if(argc == 2) {
        // Remove .h5 from input file
        char *dot  = strrchr(argv[1], '.');
        if(dot) {
            *dot = '\0';
        }
        sprintf(outfile, "%s_clean.h5", argv[1]);
    }
    else {
        sprintf(outfile, "%s", argv[2]);
    }
    write_stellar_collapse_eos_table(table, outfile);

    free(table);

    puts("All done!");
    return 0;
}
