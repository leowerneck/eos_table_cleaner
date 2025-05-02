#include <float.h>
#include <math.h>
#include <stdbool.h>

#include "basic_types.h"
#include "stellar_collapse_eos.h"
#include "utils.h"

#define INDEX(ir, it, iy) (ir + table->n_rho * (it + table->n_temperature * iy))

void
recompute_cs2_and_check_physical_limits(stellar_collapse_eos *table)
{

    bool negative_cs2     = false;
    bool superluminal_cs2 = false;
#ifdef _OPENMP
#    pragma omp parallel for collapse(3) reduction(|| : negative_cs2, superluminal_cs2)
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

                // Recompute cs2 and check physical bounds
                f64 cs2_new = bulk_modulus / rho;

                if(cs2_new < 0) {
                    warn("Found negative cs2: %g (index %lu)\n", cs2_new, index);
                    negative_cs2 = true;
                }

                // Compute the enthalpy
                const f64 h = SPEED_OF_LIGHT_SQUARED_CGS + eps + press / rho;
                if((cs2_new / h) > SPEED_OF_LIGHT_SQUARED_CGS) {
                    warn("Found superluminal cs2: %g (index %lu)\n", cs2_new / h / SPEED_OF_LIGHT_SQUARED_CGS, index);
                    superluminal_cs2 = true;
                }

                table->data[eos_cs2][index] = cs2_new / h;
            }
        }
    }
    if(!negative_cs2) {
        info("No points in the table have a negative cs2!\n");
    }
    if(!superluminal_cs2) {
        info("No points in the table have a superluminal cs2!\n");
    }
}
