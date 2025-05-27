
#include "lapacke_utils_reimpl.h" 

/* Check a vector for NaN entries. */

lapack_logical LAPACKE_hf_nancheck( lapack_int n,
                                    const lapack_float *x,
                                    lapack_int incx )
{
    lapack_int i, inc;

    if( incx == 0 ) return (lapack_logical) LAPACK_HFISNAN( x[0] );
    inc = ( incx > 0 ) ? incx : -incx ;

    for( i = 0; i < n*inc; i+=inc ) {
        if( LAPACK_HFISNAN( x[i] ) )
            return (lapack_logical) 1;
    }
    return (lapack_logical) 0;
}
