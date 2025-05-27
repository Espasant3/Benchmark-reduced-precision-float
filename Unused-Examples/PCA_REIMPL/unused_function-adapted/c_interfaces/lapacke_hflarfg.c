
#include "../include/lapacke_c_interfaces.h" 

lapack_int LAPACKE_hflarfg( lapack_int n, _Float16* alpha, _Float16* x,
                           lapack_int incx, _Float16* tau )
{
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck_reimpl() ) {
        /* Optionally check input matrices for NaNs */
        if( LAPACKE_hf_nancheck( 1, alpha, 1 ) ) {
            return -2;
        }
        if( LAPACKE_hf_nancheck( n-1, x, incx ) ) {
            return -3;
        }
    }
#endif
    return LAPACKE_hflarfg_work( n, alpha, x, incx, tau );
}
