/**
 * @file median_filter.h
 * @author Leo Werneck
 *
 * @brief Defines functions for applying a median filter to EOS table quantities.
 */
#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include "stellar_collapse_eos.h"

#define DELTASMOOTH       (10.0)                                             ///< Smoothing parameter delta.
#define MF_W              (3)                                                ///< Median filter window half-width.
#define MF_S              ((2 * MF_W + 1) * (2 * MF_W + 1) * (2 * MF_W + 1)) ///< Median filter window size.
#define INDEX(ir, it, iy) (ir + nr * (it + nt * iy))                         ///< Macro for calculating 3D index.

/**
 * @brief Applies a 3D median filter to a specified quantity in the EOS table.
 *
 * This function iterates through the EOS table data for the given quantity
 * and applies a median filter with a window size defined by MF_S.
 *
 * @param table Pointer to the stellar_collapse_eos structure containing the table data.
 * @param name The specific stellar_collapse_eos_quantity to filter.
 */
void apply_median_filter(stellar_collapse_eos *table, stellar_collapse_eos_quantity name);

#endif // MEDIAN_FILTER_H
