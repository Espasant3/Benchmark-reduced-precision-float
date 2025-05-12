
#include "../include/lapacke_c_interfaces.h"

_Float16 LAPACKE_hflansy_work( int matrix_layout, char norm, char uplo,
                                lapack_int n, const _Float16* a, lapack_int lda,
                                _Float16* work )
{
    lapack_int info = 0;
    _Float16 res = 0.0F16;
    if( matrix_layout == LAPACK_COL_MAJOR ) {
        /* Call LAPACK function and adjust info */
        res = hflansy( norm, uplo, n, a, lda, work );
        if( info < 0 ) {
            info = info - 1;
        }
    } else if( matrix_layout == LAPACK_ROW_MAJOR ) {
        lapack_int lda_t = MAX(1,n);
        _Float16* a_t = NULL;
        /* Check leading dimension(s) */
        if( lda < n ) {
            info = -6;
            LAPACKE_xerbla( "LAPACKE_hflansy_work", info );
            return info;
        }
        /* Allocate memory for temporary array(s) */
        a_t = (_Float16*)LAPACKE_malloc( sizeof(_Float16) * lda_t * MAX(1,n) );
        if( a_t == NULL ) {
            info = LAPACK_TRANSPOSE_MEMORY_ERROR;
            goto exit_level_0;
        }
        /* Transpose input matrices */
        LAPACKE_hfsy_trans( matrix_layout, uplo, n, a, lda, a_t, lda_t );
        /* Call LAPACK function and adjust info */
        res = hflansy( norm, uplo, n, a_t, lda_t, work );
        info = 0;  /* LAPACK call is ok! */
        /* Release memory and exit */
        LAPACKE_free( a_t );
exit_level_0:
        if( info == LAPACK_TRANSPOSE_MEMORY_ERROR ) {
            LAPACKE_xerbla( "LAPACKE_hflansy_work", info );
        }
    } else {
        info = -1;
        LAPACKE_xerbla( "LAPACKE_hflansy_work", info );
    }
    return res;
}
