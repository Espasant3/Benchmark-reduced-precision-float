#ifndef LAPACKE_UTILS_REIMPL_H
#define LAPACKE_UTILS_REIMPL_H

/* Incluye el archivo definitions.h */
#include "definitions.h"

/* Error handler */
void LAPACKE_xerbla( const char *name, lapack_int info );
void c_xerbla(const char *srname, int info); // Does the same but for Blas, and in case of error, it exits the program

/* Check two caracters are the same */
lapack_logical lsame_reimpl(char ca, char cb);

/* Functions to convert column-major to row-major 2d arrays and vice versa. */
void LAPACKE_hfsy_trans( int matrix_layout, char uplo, lapack_int n,
    const _Float16 *in, lapack_int ldin,
    _Float16 *out, lapack_int ldout );

void LAPACKE_hftr_trans( int matrix_layout, char uplo, char diag, lapack_int n,
    const _Float16 *in, lapack_int ldin,
    _Float16 *out, lapack_int ldout );

void LAPACKE_hfge_trans( int matrix_layout, lapack_int m, lapack_int n,
    const _Float16* in, lapack_int ldin,
    _Float16* out, lapack_int ldout );

/* Nan checker for vectors */
lapack_logical LAPACKE_hf_nancheck( lapack_int n,
    const _Float16 *x,
    lapack_int incx );

/* NaN checker for matrix */
lapack_logical LAPACKE_hfgb_nancheck( int matrix_layout, lapack_int m,
    lapack_int n, lapack_int kl,
    lapack_int ku,
    const _Float16 *ab,
    lapack_int ldab );

lapack_logical LAPACKE_hfge_nancheck( int matrix_layout, lapack_int m,
    lapack_int n,
    const _Float16 *a,
    lapack_int lda );

lapack_logical LAPACKE_hfsb_nancheck( int matrix_layout, char uplo,
    lapack_int n, lapack_int kd,
    const _Float16* ab,
    lapack_int ldab );

lapack_logical LAPACKE_hfsy_nancheck( int matrix_layout, char uplo,
    lapack_int n,
    const _Float16 *a,
    lapack_int lda );

lapack_logical LAPACKE_hftr_nancheck( int matrix_layout, char uplo, char diag,
    lapack_int n,
    const _Float16 *a,
    lapack_int lda );


/* APIs for set/get nancheck flags */
void LAPACKE_set_nancheck_reimpl(int flag);
int LAPACKE_get_nancheck_reimpl(void);



/* Main functions */
void hfsyev(char jobz, char uplo, lapack_int n, _Float16* a, lapack_int lda, _Float16* w, _Float16* work, lapack_int lwork, lapack_int* info);

lapack_int LAPACKE_hfsyev_work( int matrix_layout, char jobz, char uplo,
    lapack_int n, _Float16* a, lapack_int lda, _Float16* w,
    _Float16* work, lapack_int lwork );


/* Functions from Fortran */
// Adapted to use _Float16 instead of float

_Float16 hfdot(int n, _Float16 *sx, int incx, _Float16 *sy, int incy);

void hfgemm(char transa, char transb, int m, int n, int k,
    _Float16 alpha, const _Float16 *a, int lda,
    const _Float16 *b, int ldb, _Float16 beta,
    _Float16 *c, int ldc);

void hfgemv(char trans, int m, int n, _Float16 alpha, _Float16 *a, int lda,
    _Float16 *x, lapack_int incx, _Float16 beta, _Float16 *y, lapack_int incy);

void hfger(int m, int n, _Float16 alpha, const _Float16 *x, int incx, 
    const _Float16 *y, int incy, _Float16 *a, int lda);

void hflacpy(char uplo, int m, int n, const _Float16 *a, int lda, _Float16 *b, int ldb);

void hflae2(_Float16 a, _Float16 b, _Float16 c, _Float16 *rt1, _Float16 *rt2);

void hflaev2(_Float16 a, _Float16 b, _Float16 c, _Float16 *rt1, _Float16 *rt2, _Float16 *cs1, _Float16 *sn1);

_Float16 hflanst(char norm, int n, const _Float16 *d, const _Float16 *e);

_Float16 hflansy(char norm, char uplo, int n, const _Float16 *a, int lda, _Float16 *work);

_Float16 hflapy2(_Float16 x, _Float16 y);

void hflarfb(char side, char trans, char direct, char storev, int m, int n, int k,
    _Float16 *v, int ldv, _Float16 *t, int ldt, _Float16 *c, int ldc, _Float16 *work, int ldwork);

void hflarfg(int n, _Float16 *alpha, _Float16 *x, lapack_int incx, _Float16 *tau);

void hflarft(char direct, char storev, int n, int k, _Float16 *v, int ldv, _Float16 *tau, _Float16 *t, int ldt);

void hflarf1f(char side, int m, int n, _Float16 *v, int incv, _Float16 tau,
    _Float16 *C, int ldc, _Float16 *work);

void hflartg(_Float16 f, _Float16 g, _Float16 *c, _Float16 *s, _Float16 *r);

void hflascl(char TYPE, lapack_int KL, lapack_int KU, _Float16 CFROM, _Float16 CTO,
    lapack_int M, lapack_int N, _Float16 *A, lapack_int LDA, lapack_int *INFO);

void hflaset(char uplo, int m, int n, _Float16 alpha, _Float16 beta, _Float16* a, int lda);

void hflasr(char side, char pivot, char direct, lapack_int m, lapack_int n,
    const _Float16* c, const _Float16* s, _Float16* a, lapack_int lda);

void hflasrt(char id, lapack_int n, _Float16 *d, lapack_int *info);

void hflassq(int n, const _Float16 *x, int incx, _Float16 *scale, _Float16 *sumsq);

void hflatrd(char uplo, int n, int nb, _Float16 *a, int lda, _Float16 *e, _Float16 *tau, _Float16 *w, int ldw);

_Float16 hfnrm2(int n, _Float16 *x, int incx);

void hforgql(int m, int n, int k, _Float16 *a, int lda, _Float16 *tau, _Float16 *work, int lwork, lapack_int *info);

void hforgqr(int m, int n, int k, _Float16 *a, int lda, _Float16 *tau, _Float16 *work, int lwork, int *info);

void hforgtr(char uplo, int n, _Float16 *a, int lda, _Float16 *tau, _Float16 *work, lapack_int lwork, lapack_int *info);

void hforg2l(lapack_int m, lapack_int n, lapack_int k, _Float16 *a, lapack_int lda,
    _Float16 *tau, _Float16 *work, lapack_int *info);

void hforg2r(int m, int n, int k, _Float16 *A, int lda, 
    _Float16 *tau, _Float16 *work, int *info);

void hfscal(int n, _Float16 sa, _Float16 *sx, int incx);

void hfsteqr(const char compz, int n, _Float16 *d, _Float16 *e, _Float16 *z, int ldz, _Float16 *work, int *info);

void hfsterf(lapack_int n, _Float16 *d, _Float16 *e, int *info);

void hfswap(int n, _Float16 *sx, int incx, _Float16 *sy, int incy);

void hfswap_row_major(int n, _Float16 *sx, int incx, _Float16 *sy, int incy);

void hfswap_column_major(int n, _Float16 *sx, int incx, _Float16 *sy, int incy);

void hfsymv(char uplo, lapack_int n, _Float16 alpha, _Float16 *A, lapack_int lda,
    _Float16 *x, lapack_int incx, _Float16 beta, _Float16 *y, lapack_int incy);

void hfsyr2(char uplo, lapack_int n, _Float16 alpha, const _Float16 *x, lapack_int incx,
    const _Float16 *y, lapack_int incy, _Float16 *a, lapack_int lda);

void hfsyr2k(char uplo, char trans, int n, int k, _Float16 alpha,
    _Float16 *A, int lda, _Float16 *B, int ldb, _Float16 beta,
    _Float16 *C, int ldc);

void hfsytd2(char uplo, lapack_int n, _Float16 *a, int lda, _Float16 *d, _Float16 *e, _Float16 *tau, lapack_int *info);

void hfsytrd(char uplo, lapack_int n, _Float16 *a, lapack_int lda, _Float16 *d,
    _Float16 *e, _Float16 *tau, _Float16 *work, lapack_int lwork, lapack_int *info);

void hftrmm(const char side, const char uplo, const char transa, const char diag,
    const int m, const int n, const _Float16 alpha, const _Float16 *a, const int lda,
    _Float16 *b, const int ldb);






void hfcopy(int n, const _Float16 *sx, int incx, _Float16 *sy, int incy);

_Float16 hflamch_Float16(const char cmach);

_Float16 hfroundup_lwork(int lwork);

int ieeeck_reimpl_Float16(int ispec, _Float16 *zero, _Float16 *one);

int ilaenv_reimpl_Float16(int ispec, char* name, char* opts, int n1, int n2, int n3, int n4);

int ilaslc_reimpl_Float16(int m, int n, _Float16 *a, int lda);

int ilaslr_reimpl_Float16(int m, int n, _Float16 *a, int lda);



/* No modified functions */
// Functions that do the same as the original LAPACK functions but implemented in C

int ieeeck_reimpl(int ispec, float *zero, float *one);

int ilaenv_reimpl(int ispec, char* name, char* opts, int n1, int n2, int n3, int n4);

int iparmq_reimpl(int ispec, const char *name, const char *opts, int n, int ilo, int ihi, int lwork);

float slamch_reimpl(const char *cmach);


/* The same functions */


#endif /* LAPACKE_UTILS_REIMPL_H */
