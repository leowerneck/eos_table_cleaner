#include <stdio.h>
#include <stdlib.h>

#include "hdf5_helpers.h"
#include "stellar_collapse_eos.h"

int
main(int argc, char **argv)
{

    if(argc != 2)
    {
        fprintf(stderr, "Usage: ./eos_cleaner <input table>\n");
        exit(0);
    }

    hid_t file_id = H5Fopen(argv[1], H5F_ACC_RDONLY, H5P_DEFAULT);
    if(file_id < 0)
    {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }

    H5Fclose(file_id);

    puts("All done!");

    return 0;
}
