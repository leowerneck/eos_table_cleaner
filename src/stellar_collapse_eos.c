#include <hdf5.h>
#include <math.h>
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

static u64
validate_increasing_monotonically(const u64 size, const f64 *data, const char *name)
{
    u64 count = 0;
    for(u32 i = 1; i < size; i++) {
        const f64 left  = data[i - 1];
        const f64 right = data[i];
        if(left > right) {
            warn("%s not increasing monotonically! %g > %g\n", name, left, right);
            count++;
        }
    }
    if(count) {
        warn("Found %lu problematic points in %lu for %s\n", count, size, name);
    }
    return count;
}

#define CHECK_DATASETS_ARE_FINITE(func)                                                                                \
    for(u32 n = 0; n < number_of_eos_quantities; n++) {                                                                \
        const char *name         = dataset_names[n];                                                                   \
        u64         local_errors = 0;                                                                                  \
        for(u64 i = 0; i < size; i++) {                                                                                \
            if(isnan(table->data[n][i])) {                                                                             \
                warn("Found %s in dataset '%s', index %lu\n", #func, name, i);                                         \
                local_errors++;                                                                                        \
            }                                                                                                          \
        }                                                                                                              \
        if(local_errors) {                                                                                             \
            warn("Dataset '%s' has %lu %ss out of %lu points\n", name, local_errors, #func, size);                     \
        }                                                                                                              \
        else {                                                                                                         \
            info("Dataset '%s' does not contain %ss!\n", name, #func);                                                 \
        }                                                                                                              \
    }

void
validate_table(stellar_collapse_eos *table)
{
    const u32 nr   = table->n_rho;
    const u32 nt   = table->n_temperature;
    const u32 ny   = table->n_ye;
    const u32 size = nr * nt * ny;

    validate_increasing_monotonically(nr, table->log10_rho, "logrho");
    validate_increasing_monotonically(nt, table->log10_temperature, "logtemp");
    validate_increasing_monotonically(ny, table->ye, "ye");

    CHECK_DATASETS_ARE_FINITE(nan);
    CHECK_DATASETS_ARE_FINITE(inf);
}

// void
// recompute_derivs(stellar_collapse_eos *table)
// {
//     (void)table;
//     error(UNSUPPORTED_FEATURE, "Recompute derivatives it not yet supported.\n");
// }

char *
stellar_collapse_qty_to_str(stellar_collapse_eos_quantity qty)
{
    switch(qty) {
        case eos_Abar:
            return "Abar";
        case eos_Xa:
            return "Xa";
        case eos_Xh:
            return "Xh";
        case eos_Xn:
            return "Xn";
        case eos_Xp:
            return "Xp";
        case eos_Zbar:
            return "Zbar";
        case eos_cs2:
            return "cs2";
        case eos_dedt:
            return "dedt";
        case eos_dpderho:
            return "dpderho";
        case eos_dpdrhoe:
            return "dpdrhoe";
        case eos_entropy:
            return "entropy";
        case eos_gamma:
            return "gamma";
        case eos_logenergy:
            return "logenergy";
        case eos_logpress:
            return "logpress";
        case eos_mu_e:
            return "mu_e";
        case eos_mu_n:
            return "mu_n";
        case eos_mu_p:
            return "mu_p";
        case eos_muhat:
            return "muhat";
        case eos_munu:
            return "munu";
        default:
            return "invalid table quantity";
    }
}

