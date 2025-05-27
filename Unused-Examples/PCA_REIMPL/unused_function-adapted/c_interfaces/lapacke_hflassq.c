
#include "../include/lapacke_c_interfaces.h" 

lapack_int LAPACKE_hflassq( lapack_int n, _Float16* x, lapack_int incx, _Float16* scale, _Float16* sumsq )
{
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck_reimpl() ) {
        /* Optionally check input vector `x` and in/out scalars `scale` and `sumsq` for NaNs */
        if( LAPACKE_hf_nancheck( n, x, incx ) ) {
            return -2;
        }
        if( LAPACKE_hf_nancheck( 1, scale, 1 ) ) {
            return -4;
        }
        if( LAPACKE_hf_nancheck( 1, sumsq, 1 ) ) {
            return -5;
        }
    }
#endif
    return LAPACKE_hflassq_work( n, x, incx, scale, sumsq );
}
