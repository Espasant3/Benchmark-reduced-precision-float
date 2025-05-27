
#include "lapacke_utils_reimpl.h"

/* Check a matrix for NaN entries. */

lapack_logical LAPACKE_hfsb_nancheck( int matrix_layout, char uplo,
                                      lapack_int n, lapack_int kd,
                                      const lapack_float* ab,
                                      lapack_int ldab )
{
    if( lsame_reimpl( uplo, 'u' ) ) {
        return LAPACKE_hfgb_nancheck( matrix_layout, n, n, 0, kd, ab, ldab );
    } else if( lsame_reimpl( uplo, 'l' ) ) {
        return LAPACKE_hfgb_nancheck( matrix_layout, n, n, kd, 0, ab, ldab );
    }
    return (lapack_logical) 0;
}
