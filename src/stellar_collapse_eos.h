#ifndef STELLAR_COLLAPSE_EOS_H
#define STELLAR_COLLAPSE_EOS_H

#include "basic_types.h"

typedef enum
{
    logpress,
    logenergy,
    entropy,
    munu,
    cs2,
    dedt,
    dpdrhoe,
    dpderho,
    muhat,
    mu_e,
    mu_p,
    mu_n,
    Xa,
    Xh,
    Xn,
    Xp,
    Abar,
    Zbar,
    Gamma,
    number_of_table_quantities
} stellar_collapse_eos_quantity;

static const char *table_var_names[number_of_table_quantities] = {
    "logpress",
    "logenergy",
    "entropy",
    "munu",
    "cs2",
    "dedt",
    "dpdrhoe",
    "dpderho",
    "muhat",
    "mu_e",
    "mu_p",
    "mu_n",
    "Xa",
    "Xh",
    "Xn",
    "Xp",
    "Abar",
    "Zbar",
    "Gamma"
};

typedef struct
{
    usize n_rho, n_temperature, n_ye;
    f64  *data;
} stellar_collapse_eos;

stellar_collapse_eos *read_stellar_collapse_eos_table(const char *filepath);

void free_stellar_collapse_eos_table(stellar_collapse_eos *table);

#endif // STELLAR_COLLAPSE_EOS_H
