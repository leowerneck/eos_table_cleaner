#include "hdf5_helpers.h"
#include "basic_types.h"
#include "utils.h"

void *
read_hdf5_dataset(hid_t file_id, dataset_type dtype, const char *dataset_name)
{
    const hid_t dataset_types[2] = {H5T_NATIVE_INT, H5T_NATIVE_DOUBLE};
    const usize dtype_size[2]    = {sizeof(int), sizeof(double)};

    const hid_t hdf5_dtype = dataset_types[dtype];

    // Open the dataset
    hid_t dataset_id = H5Dopen(file_id, dataset_name, H5P_DEFAULT);
    if(dataset_id < 0) {
        error(HDF5_DATASET_NOT_FOUND, "Dataset '%s' not found.\n", dataset_name);
    }

    // Get the dataspace and dimensions
    hid_t dataspace_id = H5Dget_space(dataset_id);

    int ndims = H5Sget_simple_extent_ndims(dataspace_id);
    if(ndims < 1) {
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        error(HDF5_DATASET_INVALID_NDIMS, "Invalid number of dimensions in dataset '%s' (%d)\n", dataset_name, ndims);
    }

    hsize_t dims[ndims];
    H5Sget_simple_extent_dims(dataspace_id, dims, NULL);

    // Allocate memory for the 3D array
    size_t total_size = 1;
    for(int i = 0; i < ndims; i++) {
        total_size *= dims[i];
    }

    // We don't use malloc_or_error so we can close the HDF5 file first.
    void *array = malloc(total_size * dtype_size[dtype]);
    if(!array) {
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        error(OUT_OF_MEMORY, "Memory allocation failed for dataset '%s'.\n", dataset_name);
    }

    // Read the 3D array
    herr_t status = H5Dread(dataset_id, hdf5_dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, array);
    if(status < 0) {
        free(array);
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        error(HDF5_DATASET_READ_FAILED, "Problem reading dataset '%s'.\n", dataset_name);
    }

    // Close the dataspace and dataset
    H5Sclose(dataspace_id);
    H5Dclose(dataset_id);

    DEBUG_PRINT("Successfully read dataset '%-12s' with dimensions: {%llu", dataset_name, dims[0]);
    for(int i = 1; i < ndims; i++) {
        DEBUG_PRINT(", %llu", dims[i]);
    }
    DEBUG_PRINT("}\n");

    return array;
}

void
write_hdf5_dataset(
    hid_t          file_id,
    dataset_type   dtype,
    int            ndims,
    const hsize_t *dims,
    const void    *data,
    const char    *dataset_name
)
{
    const hid_t dataset_types[2] = {H5T_NATIVE_INT, H5T_NATIVE_DOUBLE};
    const hid_t hdf5_dtype       = dataset_types[dtype];

    // Create the dataspace for the dataset
    hid_t dataspace_id = H5Screate_simple(ndims, dims, NULL);
    if(dataspace_id < 0) {
        error(HDF5_DATASPACE_CREATE_FAILED, "Failed to create dataspace for dataset '%s'.\n", dataset_name);
    }

    // Create the dataset
    hid_t dataset_id =
        H5Dcreate(file_id, dataset_name, hdf5_dtype, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if(dataset_id < 0) {
        H5Sclose(dataspace_id);
        error(HDF5_DATASET_CREATE_FAILED, "Failed to create dataset '%s'.\n", dataset_name);
    }

    // Write the data to the dataset
    herr_t status = H5Dwrite(dataset_id, hdf5_dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if(status < 0) {
        H5Dclose(dataset_id);
        H5Sclose(dataspace_id);
        error(HDF5_DATASET_WRITE_FAILED, "Error writing dataset '%s'.\n", dataset_name);
    }

    // Close the dataset and dataspace
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);

    DEBUG_PRINT("Successfully wrote dataset '%-12s' with dimensions: {%llu", dataset_name, dims[0]);
    for(int i = 1; i < ndims; i++) {
        DEBUG_PRINT(", %llu", dims[i]);
    }
    DEBUG_PRINT("}\n");
}
