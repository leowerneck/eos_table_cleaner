#include "hdf5_helpers.h"
#include "basic_types.h"

#include <stdlib.h>

void *
read_hdf5_dataset(hid_t file_id, char *dataset_name, dataset_type dtype)
{
    const hid_t dataset_types[2] = {H5T_NATIVE_INT, H5T_NATIVE_DOUBLE};
    const usize dtype_size[2]    = {sizeof(int), sizeof(double)};
    const hid_t hdf5_dtype       = dataset_types[dtype];

    // Open the dataset
    hid_t dataset_id = H5Dopen(file_id, dataset_name, H5P_DEFAULT);
    if(dataset_id < 0)
    {
        fprintf(stderr, "Dataset '%s' not found.\n", dataset_name);
        exit(1);
    }

    // Get the dataspace and dimensions
    hid_t dataspace_id = H5Dget_space(dataset_id);

    int ndims = H5Sget_simple_extent_ndims(dataspace_id);
    if(ndims < 1)
    {
        fprintf(stderr, "Invalid number of dimensions in dataspace (%d)\n", ndims);
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        exit(2);
    }

    hsize_t dims[ndims];
    H5Sget_simple_extent_dims(dataspace_id, dims, NULL);

    // Allocate memory for the 3D array
    size_t total_size = 1;
    for(int i = 0; i < ndims; i++)
    {
        total_size *= dims[i];
    }

    void *array = malloc(total_size * dtype_size[dtype]);
    if(!array)
    {
        fprintf(stderr, "Memory allocation failed for dataset '%s'.\n", dataset_name);
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        exit(3);
    }

    // Read the 3D array
    herr_t status = H5Dread(dataset_id, hdf5_dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, array);
    if(status < 0)
    {
        fprintf(stderr, "Error reading dataset '%s'.\n", dataset_name);
        free(array);
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        exit(4);
    }

    // Close the dataspace and dataset
    H5Sclose(dataspace_id);
    H5Dclose(dataset_id);

    DEBUG_PRINT("Successfully read dataset '%-12s' with dimensions: {%llu", dataset_name, dims[0]);
    for(int i = 1; i < ndims; i++)
    {
        DEBUG_PRINT(", %llu", dims[i]);
    }
    DEBUG_PRINT("}\n");

    return array;
}
