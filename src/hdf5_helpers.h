#ifndef HDF5_HELPERS_H
#define HDF5_HELPERS_H

#include <hdf5.h>
#include <stdio.h>

typedef enum
{
    I32,
    F64
} dataset_type;

void *read_hdf5_dataset(hid_t file_id, dataset_type dtype, const char *dataset_name);

void write_hdf5_dataset(
    hid_t          file_id,
    dataset_type   dtype,
    int            ndims,
    const hsize_t *dims,
    const void    *data,
    const char    *dataset_name
);

#endif // HDF5_HELPERS_H
