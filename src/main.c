#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "median_filter.h"
#include "stellar_collapse_eos.h"
#include "utils.h"

int
main(int argc, char **argv)
{

    if(argc < 2 || argc > 3) {
        info("Usage: %s <input file> [output file]\n", argv[0]);
        return 0;
    }

    stellar_collapse_eos *table = read_stellar_collapse_eos_table(argv[1]);
    info("Successfully read table from file '%s'\n", argv[1]);

    info("Applying median filter to dPdrho_e\n");
    apply_median_filter(table, eos_dpdrhoe);

    info("Applying median filter to dPde_rho\n");
    apply_median_filter(table, eos_dpderho);

    info("Applying median filter to dedT\n");
    apply_median_filter(table, eos_dedt);

    info("Recomputing cs2\n");
    recompute_cs2(table);

    info("Validating table\n");
    validate_table(table);
    info("Validation algorithm complete! If you did not see any warnings, that\n");
    info("means all tabulated quantities in the new table are free of nans and infs!\n");
    info("In addition, it means cs2 is strictly positive and subluminal.\n");

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
    info("Successfully wrote clean table to file '%s'\n", outfile);

    free(table);

    info("All done!\n");
    return 0;
}
