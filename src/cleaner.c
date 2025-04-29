#include <stdio.h>
#include <stdlib.h>

#include <hdf5.h>

#define VERBOSE

#ifdef VERBOSE
#    define DEBUG printf
#else
#    define DEBUG (void)
#endif

size_t
read_single_integer_dataset(hid_t file_id, char *dataset_name)
{
    // Open the dataset
    hid_t dataset_id = H5Dopen(file_id, dataset_name, H5P_DEFAULT);
    if(dataset_id < 0)
    {
        fprintf(stderr, "Dataset '%s' not found.\n", dataset_name);
        return 0;
    }

    // Read the integer value
    int    value  = 0;
    herr_t status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &value);
    if(status < 0)
    {
        fprintf(stderr, "Error reading dataset '%s'.\n", dataset_name);
        H5Dclose(dataset_id);
        return 0;
    }

    DEBUG("Successfully read dataset '%-12s' with dimensions: {1}\n", dataset_name);

    // Close the dataset
    H5Dclose(dataset_id);

    return (size_t)value;
}

double *
read_double_array_dataset(hid_t file_id, char *dataset_name)
{
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

    double *array = (double *)malloc(total_size * sizeof(double));
    if(!array)
    {
        fprintf(stderr, "Memory allocation failed for dataset '%s'.\n", dataset_name);
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        exit(3);
    }

    // Read the 3D array
    herr_t status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, array);
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

    DEBUG("Successfully read dataset '%-12s' with dimensions: {%llu", dataset_name, dims[0]);
    for(int i = 1; i < ndims; i++)
    {
        DEBUG(", %llu", dims[i]);
    }
    DEBUG("}\n");

    return array;
}

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

    // Scalars
    hsize_t n_rho  = read_single_integer_dataset(file_id, "pointsrho");
    hsize_t n_temp = read_single_integer_dataset(file_id, "pointstemp");
    hsize_t n_ye   = read_single_integer_dataset(file_id, "pointsye");
    double *eps0   = read_double_array_dataset(file_id, "energy_shift");

    // Basic arrays
    double *log10_rho  = read_double_array_dataset(file_id, "logrho");
    double *log10_temp = read_double_array_dataset(file_id, "logtemp");
    double *ye         = read_double_array_dataset(file_id, "ye");

    // Core thermodynamics quantities
    double *log10_press          = read_double_array_dataset(file_id, "logpress");
    double *log10_eps_minus_eps0 = read_double_array_dataset(file_id, "logenergy");
    double *entropy              = read_double_array_dataset(file_id, "entropy");

    // Derivatives
    double *dpdrhoe = read_double_array_dataset(file_id, "dpdrhoe");
    double *dpderho = read_double_array_dataset(file_id, "dpderho");
    double *dedt    = read_double_array_dataset(file_id, "dedt");

    // Chemical potentials
    double *mu_e  = read_double_array_dataset(file_id, "mu_e");
    double *mu_n  = read_double_array_dataset(file_id, "mu_n");
    double *mu_p  = read_double_array_dataset(file_id, "mu_p");
    double *munu  = read_double_array_dataset(file_id, "munu");
    double *muhat = read_double_array_dataset(file_id, "muhat");

    // Composition
    double *Xa   = read_double_array_dataset(file_id, "Xa");
    double *Xh   = read_double_array_dataset(file_id, "Xh");
    double *Xn   = read_double_array_dataset(file_id, "Xn");
    double *Xp   = read_double_array_dataset(file_id, "Xp");
    double *Abar = read_double_array_dataset(file_id, "Abar");
    double *Zbar = read_double_array_dataset(file_id, "Zbar");

    H5Fclose(file_id);

    puts("All done!");

    return 0;
}
