#include <stdio.h>
#include <stdlib.h>

#include "stellar_collapse_eos.h"
#include "table_cleaner.h"

int
main(int argc, char **argv)
{

    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <input table>\n", argv[0]);
        exit(0);
    }

    stellar_collapse_eos *table = read_stellar_collapse_eos_table(argv[1]);

    clean_table(table);

    write_stellar_collapse_eos_table("test.h5", table);

    free(table);

    puts("All done!");
    return 0;
}
