#ifndef HDF5_HELPERS_H
#define HDF5_HELPERS_H

#include <hdf5.h>
#include <stdio.h>

typedef enum
{
    I32,
    F64
} dataset_type;

void *read_hdf5_dataset(hid_t file_id, char *dataset_name, dataset_type dtype);

#ifdef NDEBUG
#    define DEBUG_PRINT (void)
#else
#    define DEBUG_PRINT printf
#endif

#endif // HDF5_HELPERS_H
