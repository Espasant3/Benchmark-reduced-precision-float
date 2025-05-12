
#include "../include/lapacke_c_interfaces.h" 

_Float16 LAPACKE_hflansy( int matrix_layout, char norm, char uplo, lapack_int n,
                           const _Float16* a, lapack_int lda )
{
    lapack_int info = 0;
    _Float16 res = 0.0F16;
    _Float16* work = NULL;
    if( matrix_layout != LAPACK_COL_MAJOR && matrix_layout != LAPACK_ROW_MAJOR ) {
        LAPACKE_xerbla( "LAPACKE_hflansy", -1 );
        return -1;
    }
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck_reimpl() ) {
        /* Optionally check input matrices for NaNs */
        if( LAPACKE_hfsy_nancheck( matrix_layout, uplo, n, a, lda ) ) {
            return -5;
        }
    }
#endif
    /* Allocate memory for working array(s) */
    if( lsame_reimpl( norm, 'i' ) || lsame_reimpl( norm, '1' ) ||
    lsame_reimpl( norm, 'O' ) ) {
        work = (_Float16*)LAPACKE_malloc( sizeof(_Float16) * MAX(1,n) );
        if( work == NULL ) {
            info = LAPACK_WORK_MEMORY_ERROR;
            goto exit_level_0;
        }
    }
    /* Call middle-level interface */
    res = LAPACKE_hflansy_work( matrix_layout, norm, uplo, n, a, lda, work );
    /* Release memory and exit */
    if( lsame_reimpl( norm, 'i' ) || lsame_reimpl( norm, '1' ) ||
        lsame_reimpl( norm, 'O' ) ) {
        LAPACKE_free( work );
    }
exit_level_0:
    if( info == LAPACK_WORK_MEMORY_ERROR ) {
        LAPACKE_xerbla( "LAPACKE_hflansy", info );
    }
    return res;
}
