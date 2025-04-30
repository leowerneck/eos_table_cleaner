#ifndef STELLAR_COLLAPSE_EOS_H
#define STELLAR_COLLAPSE_EOS_H

#include "basic_types.h"

typedef enum
{
    eos_Abar,
    eos_Xa,
    eos_Xh,
    eos_Xn,
    eos_Xp,
    eos_Zbar,
    eos_cs2,
    eos_dedt,
    eos_dpderho,
    eos_dpdrhoe,
    eos_entropy,
    eos_gamma,
    eos_logenergy,
    eos_logpress,
    eos_mu_e,
    eos_mu_n,
    eos_mu_p,
    eos_muhat,
    eos_munu,
    number_of_eos_quantities
} stellar_collapse_eos_quantity;

typedef struct
{
    i32  n_rho, n_temperature, n_ye;
    f64 *log10_rho, *log10_temperature, *ye, energy_shift;
    f64 *data[number_of_eos_quantities];
} stellar_collapse_eos;

stellar_collapse_eos *read_stellar_collapse_eos_table(const char *filepath);

void write_stellar_collapse_eos_table(const stellar_collapse_eos *table, const char *filepath);

void free_stellar_collapse_eos_table(stellar_collapse_eos *table);

void ensure_tables_are_equal_or_error(const char *filepath1, const char *filepath2);

void recompute_cs2(stellar_collapse_eos *table);

#endif // STELLAR_COLLAPSE_EOS_H
