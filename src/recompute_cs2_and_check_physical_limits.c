#include <float.h>
#include <math.h>
#include <stdbool.h>

#include "basic_types.h"
#include "stellar_collapse_eos.h"
#include "utils.h"

#define INDEX(ir, it, iy) ((ir) + table->n_rho * ((it) + table->n_temperature * (iy)))

void
recompute_cs2_and_check_physical_limits(stellar_collapse_eos *table)
{

    i32 negative_cs2_count     = 0;
    i32 superluminal_cs2_count = 0;

#ifdef _OPENMP
#    pragma omp parallel for collapse(3) reduction(+: negative_cs2_count, superluminal_cs2_count)
#endif
    for(i32 iy = 0; iy < table->n_ye; iy++) {
        for(i32 it = 0; it < table->n_temperature; it++) {
            for(i32 ir = 0; ir < table->n_rho; ir++) {
                const u64 index    = INDEX(ir, it, iy);
                const f64 rho      = pow(10.0, table->log10_rho[ir]);
                const f64 press    = pow(10.0, table->data[eos_logpress][index]);
                const f64 eps      = pow(10.0, table->data[eos_logenergy][index]) - table->energy_shift;
                const f64 dPdrho_e = table->data[eos_dpdrhoe][index];
                const f64 dPde_rho = table->data[eos_dpderho][index];

                // assume table is hardened
                f64 bulk_modulus = rho * dPdrho_e + (press / rho) * dPde_rho;
                if(bulk_modulus < DBL_EPSILON) {
                    bulk_modulus = DBL_EPSILON;
                }

                // Recompute cs2 and check physical bounds
                const f64 h = SPEED_OF_LIGHT_SQUARED_CGS + eps + press / rho;
                const f64 w = rho * h;
                f64 cs2_new = SPEED_OF_LIGHT_SQUARED_CGS * bulk_modulus / w;

                if(cs2_new < 0) {
                    negative_cs2_count++;
                }

                // Compute the enthalpy
                if(cs2_new > SPEED_OF_LIGHT_SQUARED_CGS) {
                    superluminal_cs2_count++;
                }

                table->data[eos_cs2][index] = cs2_new;
            }
        }
    }
    if(!negative_cs2_count) {
        info("No points in the table have a negative cs2!\n");
    }
    else {
        const i32 size = table->n_rho * table->n_temperature * table->n_ye;
        warn("Found %lu points (~%.1f%%) with negative cs2! Applied ceiling (speed of light).\n",
             negative_cs2_count, 100.0 * ((double)negative_cs2_count)/((double)size));
    }
    if(!superluminal_cs2_count) {
        info("No points in the table have a superluminal cs2!\n");
    }
    else {
        const i32 size = table->n_rho * table->n_temperature * table->n_ye;
        warn("Found %lu points (~%.1f%%) with superluminal cs2! Applied ceiling (speed of light).\n",
             superluminal_cs2_count, 100.0 * ((double)superluminal_cs2_count)/((double)size));
    }
}
