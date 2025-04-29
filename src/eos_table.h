#ifndef EOS_TABLE_H
#define EOS_TABLE_H

#include "basic_types.h"

typedef enum {
    
} stellar_collapse_eos_quantity;

typedef struct {
    usize n_rho, n_temperature, n_ye;
    f64 *data;
} stellar_collapse_eos;

#endif // EOS_TABLE_H
