
#include "../include/lapacke_c_interfaces.h"

lapack_int LAPACKE_hfsterf( lapack_int n, _Float16* d, _Float16* e )
{
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck_reimpl() ) {
        /* Optionally check input matrices for NaNs */
        if( LAPACKE_hf_nancheck( n, d, 1 ) ) {
            return -2;
        }
        if( LAPACKE_hf_nancheck( n-1, e, 1 ) ) {
            return -3;
        }
    }
#endif
    return LAPACKE_hfsterf_work( n, d, e );
}
