#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>

#include "hdf5_helpers.h"
#include "stellar_collapse_eos.h"
#include "utils.h"

static const char *dataset_names[number_of_eos_quantities] = {
    "Abar",    "Xa",    "Xh",        "Xn",       "Xp",   "Zbar", "cs2",  "dedt",  "dpderho", "dpdrhoe",
    "entropy", "gamma", "logenergy", "logpress", "mu_e", "mu_n", "mu_p", "muhat", "munu",
};

stellar_collapse_eos *
read_stellar_collapse_eos_table(const char *filepath)
{
    hid_t file_id = H5Fopen(filepath, H5F_ACC_RDONLY, H5P_DEFAULT);
    if(file_id < 0) {
        error(FILE_OPEN_FAILED, "Could not open file '%s'\n", filepath);
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
    for(u32 n = 0; n < number_of_eos_quantities; n++) {
        table->data[n] = (f64 *)read_hdf5_dataset(file_id, F64, dataset_names[n]);
    }

    H5Fclose(file_id);

    return table;
}

void
write_stellar_collapse_eos_table(const stellar_collapse_eos *table, const char *filepath)
{
    hid_t file_id = H5Fcreate(filepath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if(file_id < 0) {
        error(FILE_OPEN_FAILED, "Could not open file '%s'\n", filepath);
    }

    const hsize_t dims[4] = {1, table->n_ye, table->n_temperature, table->n_rho};

    // Scalar quantities
    write_hdf5_dataset(file_id, I32, 1, dims, &table->n_rho, "pointsrho");
    write_hdf5_dataset(file_id, I32, 1, dims, &table->n_temperature, "pointstemp");
    write_hdf5_dataset(file_id, I32, 1, dims, &table->n_ye, "pointsye");
    write_hdf5_dataset(file_id, F64, 1, dims, &table->energy_shift, "energy_shift");

    // Basic tabulated quantities
    write_hdf5_dataset(file_id, F64, 1, dims + 1, table->ye, "ye");
    write_hdf5_dataset(file_id, F64, 1, dims + 2, table->log10_temperature, "logtemp");
    write_hdf5_dataset(file_id, F64, 1, dims + 3, table->log10_rho, "logrho");

    // Tabulated data
    for(u32 n = 0; n < number_of_eos_quantities; n++) {
        write_hdf5_dataset(file_id, F64, 3, dims + 1, table->data[n], dataset_names[n]);
    }
}

void
free_stellar_collapse_eos_table(stellar_collapse_eos *table)
{
    if(!table) {
        return;
    }
    for(u32 n = 0; n < number_of_eos_quantities; n++) {
        if(table->data[n]) {
            free(table->data[n]);
        }
    }
    free(table);
}

#define CHECK_SCALAR(name)                                                                                             \
    if(table1->name != table2->name) {                                                                                 \
        warn("Error in %s: %g != %g\n", #name, (f64)table1->name, (f64)table2->name);                                  \
    }

void
ensure_tables_are_equal_or_error(const char *filepath1, const char *filepath2)
{
    stellar_collapse_eos *table1 = read_stellar_collapse_eos_table(filepath1);
    stellar_collapse_eos *table2 = read_stellar_collapse_eos_table(filepath2);

    CHECK_SCALAR(n_rho);
    CHECK_SCALAR(n_temperature);
    CHECK_SCALAR(n_ye);
    CHECK_SCALAR(energy_shift);

    const u32 size             = table1->n_rho * table1->n_temperature * table1->n_ye;
    bool      all_tests_passed = true;
    for(u32 n = 0; n < number_of_eos_quantities; n++) {
        debug("Validating dataset '%-9s'\n", dataset_names[n]);
        for(u32 i = 0; i < size; i++) {
            if(table1->data[n][i] != table2->data[n][i]) {
                warn("Error in %s: %g != %g\n", dataset_names[n], table1->data[n][i], table2->data[n][i]);
                all_tests_passed = false;
            }
        }
    }

    free_stellar_collapse_eos_table(table1);
    free_stellar_collapse_eos_table(table2);

    if(all_tests_passed) {
        info("All tests passed!\n");
    }
}
