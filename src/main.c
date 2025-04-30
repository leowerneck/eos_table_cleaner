#include <stdio.h>
#include <stdlib.h>

#include "stellar_collapse_eos.h"
#include "median_filter.h"

int
main(int argc, char **argv)
{

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <input table>\n", argv[0]);
        exit(0);
    }

    stellar_collapse_eos *table = read_stellar_collapse_eos_table(argv[1]);

    APPLY_MEDIAN_FILTER(table, dpdrhoe);
    APPLY_MEDIAN_FILTER(table, dpderho);
    APPLY_MEDIAN_FILTER(table, dedt);

    write_stellar_collapse_eos_table("test.h5", table);

    free(table);

    ensure_tables_are_equal_or_error(argv[1], "test.h5");

    puts("All done!");
    return 0;
}
