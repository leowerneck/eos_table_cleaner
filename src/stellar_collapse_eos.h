#ifndef STELLAR_COLLAPSE_EOS_H
#define STELLAR_COLLAPSE_EOS_H

#include "basic_types.h"

typedef enum
{
    Abar,
    X3he,
    X4li,
    Xa,
    Xd,
    Xh,
    Xn,
    Xp,
    Xt,
    Zbar,
    cs2,
    dedt,
    dpderho,
    dpdrhoe,
    entropy,
    gamma,
    logenergy,
    logpress,
    mu_e,
    mu_n,
    mu_p,
    muhat,
    munu,
    number_of_table_quantities
} stellar_collapse_eos_quantity;

typedef struct
{
    i32  n_rho, n_temperature, n_ye;
    f64 *log10_rho, *log10_temperature, *ye, energy_shift;
    f64 *data[number_of_table_quantities];
} stellar_collapse_eos;

stellar_collapse_eos *read_stellar_collapse_eos_table(const char *filepath);

void write_stellar_collapse_eos_table(const char *filepath, const stellar_collapse_eos *table);

void free_stellar_collapse_eos_table(stellar_collapse_eos *table);

void ensure_tables_are_equal_or_error(const char *filepath1, const char *filepath2);

#endif // STELLAR_COLLAPSE_EOS_H
