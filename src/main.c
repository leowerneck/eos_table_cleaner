#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "median_filter.h"
#include "options.h"
#include "stellar_collapse_eos.h"
#include "utils.h"

int
main(int argc, char **argv)
{

    if(argc < 2 || (argc % 2 != 0)) {
        info(
            "Usage: %s [-o <outfile>] [-s <smoothing>] [-d <derivs>] <input>\n"
            "  -o, --output      Output file name. Default <input>_clean.h5\n"
            "  -s, --smoothing   derivs (default), hydro, all, none (for debugging)\n"
            "  -d, --derivs      smooth (default), recompute, none (for debugging)\n",
            argv[0]
        );
        return 0;
    }
    options_t opts = parse_cmd_args(argc, argv);
    if(opts.derivs == DERIVS_RECOMPUTE) {
        error(UNSUPPORTED_FEATURE, "Recompute derivatives it not yet supported.\n");
    }

    stellar_collapse_eos *table = read_stellar_collapse_eos_table(opts.input_table_path);
    info("Successfully read table from file '%s'\n", opts.input_table_path);

    if(opts.smoother == SMOOTH_ALL) {
        info("Applying median filter to *all* table quantities:\n");
        for(int qty = 0; qty < number_of_eos_quantities; qty++) {
            if(opts.derivs != DERIVS_SMOOTH && (qty == eos_dpdrhoe || qty == eos_dpderho || qty == eos_dedt)) {
                continue;
            }
            info("  %s...\n", stellar_collapse_qty_to_str(qty));
            apply_median_filter(table, qty);
        }
    }
    else if(opts.smoother == SMOOTH_HYDRO_ONLY) {
        info("Applying median filter to hydro quantities only (not derivatives)\n");
        for(int qty = 0; qty < number_of_eos_quantities; qty++) {
            if(qty == eos_dpdrhoe || qty == eos_dpderho || qty == eos_dedt) {
                continue;
            }
            info("  %s...\n", stellar_collapse_qty_to_str(qty));
            apply_median_filter(table, qty);
        }
    }
    else if(opts.smoother == SMOOTH_DERIVS_ONLY) {
        info("Applying median filter to derivatives only\n");

        info("  dpdrhoe...\n");
        apply_median_filter(table, eos_dpdrhoe);

        info("  dpderho...\n");
        apply_median_filter(table, eos_dpderho);

        info("  dedt...\n");
        apply_median_filter(table, eos_dedt);
    }

    // if(opts.derivs == DERIVS_RECOMPUTE) {
    //     recompute_derivs(table);
    // }

    info("Recomputing cs2\n");
    recompute_cs2_and_check_physical_limits(table);

    info("Validating table\n");
    validate_table(table);

    write_stellar_collapse_eos_table(table, opts.output_table_path);
    info("Successfully wrote clean table to file '%s'\n", opts.output_table_path);

    free(table);

    info("All done!\n");
    return 0;
}
