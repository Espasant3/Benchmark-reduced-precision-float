#ifndef LAPACKE_C_INTERFACES_H
#define LAPACKE_C_INTERFACES_H

#include "definitions.h"
#include "lapacke_utils_reimpl.h"


_Float16 LAPACKE_hflansy( int matrix_layout, char norm, char uplo, lapack_int n,
    const _Float16* a, lapack_int lda );

_Float16 LAPACKE_hflansy_work( int matrix_layout, char norm, char uplo,
    lapack_int n, const _Float16* a, lapack_int lda,
    _Float16* work );

_Float16 LAPACKE_hflapy2( _Float16 x, _Float16 y );

_Float16 LAPACKE_hflapy2_work( _Float16 x, _Float16 y );

lapack_int LAPACKE_hflarfg( lapack_int n, _Float16* alpha, _Float16* x,
    lapack_int incx, _Float16* tau );

lapack_int LAPACKE_hflarfg_work( lapack_int n, _Float16* alpha, _Float16* x,
    lapack_int incx, _Float16* tau );

lapack_int LAPACKE_hflascl( int matrix_layout, char type, lapack_int kl,
    lapack_int ku, float cfrom, float cto,
    lapack_int m, lapack_int n, _Float16* a,
    lapack_int lda );

lapack_int LAPACKE_hflascl_work( int matrix_layout, char type, lapack_int kl,
    lapack_int ku, float cfrom, float cto,
    lapack_int m, lapack_int n, _Float16* a,
    lapack_int lda );

lapack_int LAPACKE_hflassq( lapack_int n, _Float16* x, lapack_int incx, _Float16* scale, _Float16* sumsq );

lapack_int LAPACKE_hflassq_work( lapack_int n, _Float16* x, lapack_int incx, _Float16* scale, _Float16* sumsq );

lapack_int LAPACKE_hforgtr( int matrix_layout, char uplo, lapack_int n, _Float16* a,
    lapack_int lda, _Float16* tau );

lapack_int LAPACKE_hforgtr_work( int matrix_layout, char uplo, lapack_int n,
    _Float16* a, lapack_int lda, _Float16* tau,
    _Float16* work, lapack_int lwork );

lapack_int LAPACKE_hfsteqr( int matrix_layout, char compz, lapack_int n, _Float16* d,
    _Float16* e, _Float16* z, lapack_int ldz );

lapack_int LAPACKE_hfsteqr_work( int matrix_layout, char compz, lapack_int n,
    _Float16* d, _Float16* e, _Float16* z, lapack_int ldz,
    _Float16* work );

lapack_int LAPACKE_hfsterf( lapack_int n, _Float16* d, _Float16* e );

lapack_int LAPACKE_hfsterf_work( lapack_int n, _Float16* d, _Float16* e );

lapack_int LAPACKE_hfsyev_work( int matrix_layout, char jobz, char uplo,
    lapack_int n, _Float16* a, lapack_int lda, _Float16* w,
    _Float16* work, lapack_int lwork );

lapack_int LAPACKE_hfsytrd( int matrix_layout, char uplo, lapack_int n, _Float16* a,
    lapack_int lda, _Float16* d, _Float16* e, _Float16* tau );

lapack_int LAPACKE_hfsytrd_work( int matrix_layout, char uplo, lapack_int n,
    _Float16* a, lapack_int lda, _Float16* d, _Float16* e,
    _Float16* tau, _Float16* work, lapack_int lwork );


#endif /* LAPACKE_C_INTERFACES_H */