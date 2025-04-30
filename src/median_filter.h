#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include "basic_types.h"
#include "stellar_collapse_eos.h"

#define DELTASMOOTH       (10.0)
#define MF_W              (3)
#define MF_S              ((2 * MF_W + 1) * (2 * MF_W + 1) * (2 * MF_W + 1))
#define INDEX(ir, it, iy) (ir + nr * (it + nt * iy))
#define APPLY_MEDIAN_FILTER(table, deriv)                                                                              \
    DEBUG_PRINT("Applying median filter to %s.....", #deriv);                                                          \
    DEBUG_FLUSH(stdout);                                                                                               \
    median_filter(table->n_rho, table->n_temperature, table->n_ye, table->data[deriv]);                                \
    DEBUG_PRINT("OK\n");

void median_filter(const u32 nr, const u32 nt, const u32 ny, f64 *deriv);

#endif // MEDIAN_FILTER_H
