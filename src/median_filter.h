#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include "stellar_collapse_eos.h"

#define DELTASMOOTH       (10.0)
#define MF_W              (3)
#define MF_S              ((2 * MF_W + 1) * (2 * MF_W + 1) * (2 * MF_W + 1))
#define INDEX(ir, it, iy) (ir + nr * (it + nt * iy))

void apply_median_filter(stellar_collapse_eos *table, stellar_collapse_eos_quantity name);

#endif // MEDIAN_FILTER_H
