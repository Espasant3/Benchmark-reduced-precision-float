#include "lapacke_utils_reimpl.h" 

/* Check a matrix for NaN entries. */

lapack_logical LAPACKE_hfgb_nancheck( int matrix_layout, lapack_int m,
                                      lapack_int n, lapack_int kl,
                                      lapack_int ku,
                                      const lapack_float *ab,
                                      lapack_int ldab )
{
    lapack_int i, j;

    if( ab == NULL ) return (lapack_logical) 0;

    if( matrix_layout == LAPACK_COL_MAJOR ) {
        for( j = 0; j < n; j++ ) {
            for( i = MAX( ku-j, 0 ); i < MIN( m+ku-j, kl+ku+1 );
                 i++ ) {
                if( LAPACK_HFISNAN( ab[i+(size_t)j*ldab] ) )
                     return (lapack_logical) 1;
            }
        }
    } else if ( matrix_layout == LAPACK_ROW_MAJOR ) {
        for( j = 0; j < n; j++ ) {
            for( i = MAX( ku-j, 0 ); i < MIN( m+ku-j, kl+ku+1 ); i++ ) {
                if( LAPACK_HFISNAN( ab[(size_t)i*ldab+j] ) )
                     return (lapack_logical) 1;
            }
        }
    }
    return (lapack_logical) 0;
}
