
#include "../include/lapacke_c_interfaces.h"

lapack_int LAPACKE_hforgtr_work( int matrix_layout, char uplo, lapack_int n,
                                _Float16* a, lapack_int lda, _Float16* tau,
                                _Float16* work, lapack_int lwork )
{
    lapack_int info = 0;
    if( matrix_layout == LAPACK_COL_MAJOR ) {
        /* Call LAPACK function and adjust info */
        hforgtr( uplo, n, a, lda, tau, work, lwork, &info );
        if( info < 0 ) {
            info = info - 1;
        }
    } else if( matrix_layout == LAPACK_ROW_MAJOR ) {
        lapack_int lda_t = MAX(1,n);
        _Float16* a_t = NULL;
        /* Check leading dimension(s) */
        if( lda < n ) {
            info = -5;
            LAPACKE_xerbla( "LAPACKE_hforgtr_work", info );
            return info;
        }
        /* Query optimal working array(s) size if requested */
        if( lwork == -1 ) {
            hforgtr( uplo, n, a, lda_t, tau, work, lwork, &info );
            return (info < 0) ? (info - 1) : info;
        }
        /* Allocate memory for temporary array(s) */
        a_t = (_Float16*)LAPACKE_malloc( sizeof(_Float16) * lda_t * MAX(1,n) );
        if( a_t == NULL ) {
            info = LAPACK_TRANSPOSE_MEMORY_ERROR;
            goto exit_level_0;
        }
        /* Transpose input matrices */
        LAPACKE_hfge_trans( matrix_layout, n, n, a, lda, a_t, lda_t );
        /* Call LAPACK function and adjust info */
        hforgtr( uplo, n, a_t, lda_t, tau, work, lwork, &info );
        if( info < 0 ) {
            info = info - 1;
        }
        /* Transpose output matrices */
        LAPACKE_hfge_trans( LAPACK_COL_MAJOR, n, n, a_t, lda_t, a, lda );
        /* Release memory and exit */
        LAPACKE_free( a_t );
exit_level_0:
        if( info == LAPACK_TRANSPOSE_MEMORY_ERROR ) {
            LAPACKE_xerbla( "LAPACKE_hforgtr_work", info );
        }
    } else {
        info = -1;
        LAPACKE_xerbla( "LAPACKE_hforgtr_work", info );
    }
    return info;
}
