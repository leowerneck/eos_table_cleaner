/**
 * @file stellar_collapse_eos.h
 * @author Leo Werneck
 *
 * @brief Defines structures and functions for handling stellar collapse equation of state (EOS) tables.
 */
#ifndef STELLAR_COLLAPSE_EOS_H
#define STELLAR_COLLAPSE_EOS_H

#include "basic_types.h"

/**
 * @brief Enum defining the various quantities available in the EOS table.
 */
typedef enum
{
    eos_Abar,                ///< Average mass number A_h of heavy nuclei
    eos_Xa,                  ///< Mass fraction of alpha particles
    eos_Xh,                  ///< Mass fraction of heavy nuclei
    eos_Xn,                  ///< Mass fraction of free neutrons
    eos_Xp,                  ///< Mass fraction of free protons
    eos_Zbar,                ///< Average charge number Z_h of heavy nuclei
    eos_cs2,                 ///< Sound speed squared, c_s^2
    eos_dedt,                ///< Derivative of specific internal energy with respect to temperature
    eos_dpderho,             ///< Derivative of pressure with respect to specific internal energy at constant density
    eos_dpdrhoe,             ///< Derivative of pressure with respect to density at constant specific internal energy
    eos_entropy,             ///< Specific entropy per baryon
    eos_gamma,               ///< Adiabatic index
    eos_logenergy,           ///< Log10 of specific internal energy, log10(eps+energy_shift)
    eos_logpress,            ///< Log10 of pressure, log10(P)
    eos_mu_e,                ///< Electron chemical potential, mu_e
    eos_mu_n,                ///< Neutron chemical potential, mu_n
    eos_mu_p,                ///< Proton chemical potential, mu_p
    eos_muhat,               ///< Chemical potential difference, mu_hat = mu_n - mu_p
    eos_munu,                ///< Neutrino chemical potential, mu_nu = mu_n - mu_p + mu_e
    number_of_eos_quantities ///< Total number of EOS quantities
} stellar_collapse_eos_quantity;

/**
 * @brief Structure representing a stellar collapse EOS table.
 *
 * Stores the grid dimensions, grid points, and data arrays for all EOS quantities.
 */
typedef struct
{
    i32  n_rho, n_temperature, n_ye;         ///< Number of grid points in density, temperature, and electron fraction.
    f64 *log10_rho, *log10_temperature, *ye; ///< Arrays storing the grid points for log10(rho), log10(T), and Ye.
    f64  energy_shift;                       ///< Energy shift applied to the specific internal energy.
    f64 *data[number_of_eos_quantities];     ///< Array of pointers to the data for each EOS quantity.
} stellar_collapse_eos;

/**
 * @brief Reads a stellar collapse EOS table from a file.
 *
 * Allocates memory for the table structure and reads the data from the specified HDF5 file.
 *
 * @param filepath Path to the EOS table file (HDF5 format).
 *
 * @return Pointer to the allocated and populated stellar_collapse_eos structure, or NULL on failure.
 */
stellar_collapse_eos *read_stellar_collapse_eos_table(const char *filepath);

/**
 * @brief Writes a stellar collapse EOS table to a file.
 *
 * Writes the data contained in the provided table structure to a new HDF5 file.
 *
 * @param table Pointer to the stellar_collapse_eos structure to write.
 * @param filepath Path to the output EOS table file (HDF5 format).
 */
void write_stellar_collapse_eos_table(const stellar_collapse_eos *table, const char *filepath);

/**
 * @brief Frees the memory allocated for a stellar collapse EOS table.
 *
 * Deallocates all memory associated with the table structure, including grid point arrays and data arrays.
 *
 * @param table Pointer to the stellar_collapse_eos structure to free.
 */
void free_stellar_collapse_eos_table(stellar_collapse_eos *table);

/**
 * @brief Compares two EOS table files for equality and exits with an error if they differ.
 *
 * Reads both tables and performs a detailed comparison of their metadata and data content.
 *
 * @param filepath1 Path to the first EOS table file.
 * @param filepath2 Path to the second EOS table file.
 */
void ensure_tables_are_equal_or_error(const char *filepath1, const char *filepath2);

/**
 * @brief Recomputes the sound speed squared (cs2) from other thermodynamic quantities.
 *
 * Uses the formula cs2 = ( rho * dpdrhoe + (P / rho) * dpderho ) / (rho * h)  to recalculate cs2. The enthalpy is
 * computed using h = 1 + eps + P / rho. Assumes that P, eps, dpdrhoe, and dpderho are already present in the table.
 *
 * @param table Pointer to the stellar_collapse_eos structure where cs2 will be recomputed.
 */
void recompute_cs2(stellar_collapse_eos *table);

#endif // STELLAR_COLLAPSE_EOS_H
