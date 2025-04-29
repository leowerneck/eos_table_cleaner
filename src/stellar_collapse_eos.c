#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "hdf5_helpers.h"
#include "stellar_collapse_eos.h"

static const char *dataset_names[number_of_table_quantities] = {
    "Abar",    "X3he",    "X4li",    "Xa",    "Xd",        "Xh",       "Xn",   "Xp",   "Xt",   "Zbar",  "cs2",  "dedt",
    "dpderho", "dpdrhoe", "entropy", "gamma", "logenergy", "logpress", "mu_e", "mu_n", "mu_p", "muhat", "munu",
};

stellar_collapse_eos *
read_stellar_collapse_eos_table(const char *filepath)
{
    hid_t file_id = H5Fopen(filepath, H5F_ACC_RDONLY, H5P_DEFAULT);
    if(file_id < 0)
    {
        fprintf(stderr, "Error: could not open file '%s'\n", filepath);
        exit(1);
    }

    stellar_collapse_eos *table = malloc_or_error(sizeof(stellar_collapse_eos));

    // Scalar quantities
    table->n_rho         = *(i32 *)read_hdf5_dataset(file_id, I32, "pointsrho");
    table->n_temperature = *(i32 *)read_hdf5_dataset(file_id, I32, "pointstemp");
    table->n_ye          = *(i32 *)read_hdf5_dataset(file_id, I32, "pointsye");
    table->energy_shift  = *(f64 *)read_hdf5_dataset(file_id, F64, "energy_shift");

    // Basic tabulated quantities
    table->ye                = (f64 *)read_hdf5_dataset(file_id, F64, "ye");
    table->log10_temperature = (f64 *)read_hdf5_dataset(file_id, F64, "logtemp");
    table->log10_rho         = (f64 *)read_hdf5_dataset(file_id, F64, "logrho");

    // Tabulated data
    for(u32 n = 0; n < number_of_table_quantities; n++)
    {
        table->data[n] = (f64 *)read_hdf5_dataset(file_id, F64, dataset_names[n]);
    }

    H5Fclose(file_id);

    return table;
}

void
write_stellar_collapse_eos_table(const char *filepath, const stellar_collapse_eos *table)
{
    hid_t file_id = H5Fcreate(filepath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if(file_id < 0)
    {
        fprintf(stderr, "Error: could not open file '%s'\n", filepath);
        exit(1);
    }

    const hsize_t dims[4] = {1, table->n_ye, table->n_temperature, table->n_rho};

    // Scalar quantities
    write_hdf5_dataset(file_id, I32, 1, dims, &table->n_rho, "pointsrho");
    write_hdf5_dataset(file_id, I32, 1, dims, &table->n_temperature, "pointstemp");
    write_hdf5_dataset(file_id, I32, 1, dims, &table->n_ye, "pointsye");
    write_hdf5_dataset(file_id, I32, 1, dims, &table->energy_shift, "energy_shift");

    // Basic tabulated quantities
    write_hdf5_dataset(file_id, F64, 1, dims + 1, table->ye, "ye");
    write_hdf5_dataset(file_id, F64, 1, dims + 2, table->log10_temperature, "logtemp");
    write_hdf5_dataset(file_id, F64, 1, dims + 3, table->log10_rho, "logrho");

    // Tabulated data
    for(u32 n = 0; n < number_of_table_quantities; n++)
    {
        write_hdf5_dataset(file_id, F64, 3, dims + 1, table->data[n], dataset_names[n]);
    }

}
