#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "basic_types.h"
#include "median_filter.h"
#include "utils.h"

static inline i32
compare_f64(const void *ap, const void *bp)
{
    double a = *((f64 *)ap);
    double b = *((f64 *)bp);
    return (a > b) - (a < b);
}

static void
median_filter_fill_buffer(u32 nr, u32 nt, i32 width, u32 ir, u32 it, u32 iy, f64 *deriv, f64 *buffer)
{
    u32 i = 0;
    for(i32 iWy = -width; iWy <= width; iWy++) {
        for(i32 iWt = -width; iWt <= width; iWt++) {
            for(i32 iWr = -width; iWr <= width; iWr++) {
                buffer[i++] = deriv[INDEX(ir + iWr, it + iWt, iy + iWy)];
            }
        }
    }
}

static f64
median_filter_find_median(const u32 size, f64 *buffer)
{
    qsort(buffer, size, sizeof(f64), compare_f64);

    if(size % 2 != 0) {
        return buffer[size / 2];
    }

    return 0.5 * (buffer[(size - 1) / 2] + buffer[size / 2]);
}

void
apply_median_filter(stellar_collapse_eos *table, stellar_collapse_eos_quantity name)
{

    const u32 nr    = table->n_rho;
    const u32 nt    = table->n_temperature;
    const u32 ny    = table->n_ye;
    f64      *deriv = table->data[name];

    size_t size = sizeof(f64) * nr * nt * ny;
    f64   *in   = (f64 *)malloc_or_error(size);
    memcpy(in, deriv, size);

    // filter, overwriting as needed
#pragma omp parallel for collapse(3)
    for(u32 iy = MF_W; iy < ny - MF_W; ++iy) {
        for(u32 it = MF_W; it < nt - MF_W; ++it) {
            for(u32 ir = MF_W; ir < nr - MF_W; ++ir) {
                f64       buffer[MF_S];
                const u32 index = INDEX(ir, it, iy);
                median_filter_fill_buffer(nr, nt, MF_W, ir, it, iy, in, buffer);
                const f64  avg = median_filter_find_median(MF_S, buffer);
                const bool bad = fabs(avg - in[index]) / fabs(avg) > DELTASMOOTH;
                if(bad) {
                    deriv[index] = avg;
                }
            }
        }
    }
    free(in);
}
