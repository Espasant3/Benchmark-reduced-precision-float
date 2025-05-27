
#include "../include/lapacke_c_interfaces.h"

lapack_int LAPACKE_hfsytrd( int matrix_layout, char uplo, lapack_int n, _Float16* a,
                           lapack_int lda, _Float16* d, _Float16* e, _Float16* tau )
{
    lapack_int info = 0;
    lapack_int lwork = -1;
    _Float16* work = NULL;
    _Float16 work_query;
    if( matrix_layout != LAPACK_COL_MAJOR && matrix_layout != LAPACK_ROW_MAJOR ) {
        LAPACKE_xerbla( "LAPACKE_hfsytrd", -1 );
        return -1;
    }
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck_reimpl() ) {
        /* Optionally check input matrices for NaNs */
        if( LAPACKE_hfsy_nancheck( matrix_layout, uplo, n, a, lda ) ) {
            return -4;
        }
    }
#endif
    /* Query optimal working array(s) size */
    info = LAPACKE_hfsytrd_work( matrix_layout, uplo, n, a, lda, d, e, tau,
                                &work_query, lwork );
    if( info != 0 ) {
        goto exit_level_0;
    }
    lwork = (lapack_int)work_query;
    /* Allocate memory for work arrays */
    work = (_Float16*)LAPACKE_malloc( sizeof(_Float16) * lwork );
    if( work == NULL ) {
        info = LAPACK_WORK_MEMORY_ERROR;
        goto exit_level_0;
    }
    /* Call middle-level interface */
    info = LAPACKE_hfsytrd_work( matrix_layout, uplo, n, a, lda, d, e, tau, work,
                                lwork );
    /* Release memory and exit */
    LAPACKE_free( work );
exit_level_0:
    if( info == LAPACK_WORK_MEMORY_ERROR ) {
        LAPACKE_xerbla( "LAPACKE_ssytrd", info );
    }
    return info;
}
