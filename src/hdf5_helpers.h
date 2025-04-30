/**
 * @file hdf5_helpers.h
 * @author Leo Werneck
 *
 * @brief Helper functions for reading and writing datasets in HDF5 files.
 */

#ifndef HDF5_HELPERS_H
#define HDF5_HELPERS_H

#include <hdf5.h>

/**
 * @brief Enumeration for supported HDF5 dataset types.
 */
typedef enum
{
    I32,
    F64
} dataset_type;

/**
 * @brief Reads an HDF5 dataset from a file.
 *
 * @param file_id The HDF5 file identifier.
 * @param dtype The data type of the dataset (I32 or F64).
 * @param dataset_name The name of the dataset to read.
 *
 * @return A pointer to the allocated memory containing the dataset data.
 *         The caller is responsible for freeing this memory. Returns NULL on error.
 */
void *read_hdf5_dataset(hid_t file_id, dataset_type dtype, const char *dataset_name);

/**
 * @brief Writes data to an HDF5 dataset in a file.
 *
 * @param file_id The HDF5 file identifier.
 * @param dtype The data type of the dataset (I32 or F64).
 * @param ndims The number of dimensions of the dataset.
 * @param dims An array containing the size of each dimension.
 * @param data A pointer to the data to be written.
 * @param dataset_name The name of the dataset to write.
 *
 * @note If the dataset already exists, it will be overwritten.
 *
 */
void write_hdf5_dataset(
    hid_t          file_id,
    dataset_type   dtype,
    int            ndims,
    const hsize_t *dims,
    const void    *data,
    const char    *dataset_name
);

#endif // HDF5_HELPERS_H
