
#include "../include/lapacke_c_interfaces.h"

_Float16 LAPACKE_hflapy2( _Float16 x, _Float16 y )
{
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck_reimpl() ) {
        /* Optionally check input matrices for NaNs */
        if( LAPACKE_hf_nancheck( 1, &x, 1 ) ) {
            return -1;
        }
        if( LAPACKE_hf_nancheck( 1, &y, 1 ) ) {
            return -2;
        }
    }
#endif
    return LAPACKE_hflapy2_work( x, y );
}
