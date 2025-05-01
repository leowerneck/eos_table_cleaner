#include <float.h>
#include <math.h>
#include <stdbool.h>

#include "basic_types.h"
#include "stellar_collapse_eos.h"

#define INDEX(ir, it, iy) (ir + table->n_rho * (it + table->n_temperature * iy))

void
recompute_cs2(stellar_collapse_eos *table)
{
    // TODO(LRW): detect this from the table, if possible, or make it an option
    const bool relativistic_cs2 = true;

#ifdef _OPENMP
#pragma omp parallel for collapse(3)
#endif
    for(i32 iy = 0; iy < table->n_ye; ++iy) {
        for(i32 it = 0; it < table->n_temperature; ++it) {
            for(i32 ir = 0; ir < table->n_rho; ++ir) {
                const u64 index    = INDEX(ir, it, iy);
                const f64 rho      = pow(10.0, table->log10_rho[ir]);
                const f64 press    = pow(10.0, table->data[eos_logpress][index]);
                const f64 eps      = pow(10.0, table->data[eos_logenergy][index]);
                const f64 dPdrho_e = table->data[eos_dpdrhoe][index];
                const f64 dPde_rho = table->data[eos_dpderho][index];

                // assume table is hardened
                f64 bulk_modulus = rho * dPdrho_e + (press / rho) * dPde_rho;
                if(bulk_modulus < DBL_EPSILON) {
                    bulk_modulus = DBL_EPSILON;
                }

                // Compute the enthalp
                const f64 h = SPEED_OF_LIGHT_SQUARED_CGS + eps + press / rho;

                // Recompute cs2
                f64 cs2_new = bulk_modulus / rho;
                if(relativistic_cs2) {
                    cs2_new /= h;
                }
                table->data[eos_cs2][index] = cs2_new;
            }
        }
    }
}
