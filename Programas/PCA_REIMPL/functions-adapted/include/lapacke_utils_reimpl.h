/*
 * Trabajo Fin de Grado - Universidad de Santiago de Compostela
 * Autor: Eloi Barcón Piñeiro
 * Año: 2025
 * 
 * Licencia MIT (ver ../../../../LICENSE_MIT)
 */

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
    const lapack_float *in, lapack_int ldin,
    lapack_float *out, lapack_int ldout );

void LAPACKE_hftr_trans( int matrix_layout, char uplo, char diag, lapack_int n,
    const lapack_float *in, lapack_int ldin,
    lapack_float *out, lapack_int ldout );

void LAPACKE_hfge_trans( int matrix_layout, lapack_int m, lapack_int n,
    const lapack_float* in, lapack_int ldin,
    lapack_float* out, lapack_int ldout );

/* NaN checker for matrix */
lapack_logical LAPACKE_hfsy_nancheck( int matrix_layout, char uplo,
    lapack_int n,
    const lapack_float *a,
    lapack_int lda );

lapack_logical LAPACKE_hftr_nancheck( int matrix_layout, char uplo, char diag,
    lapack_int n,
    const lapack_float *a,
    lapack_int lda );


/* APIs for set/get nancheck flags */
void LAPACKE_set_nancheck_reimpl(int flag);
int LAPACKE_get_nancheck_reimpl(void);



/* Main functions */
void hfsyev(char jobz, char uplo, lapack_int n, lapack_float* a, lapack_int lda, lapack_float* w, lapack_float* work, lapack_int lwork, lapack_int* info);

lapack_int LAPACKE_hfsyev_work( int matrix_layout, char jobz, char uplo,
    lapack_int n, lapack_float* a, lapack_int lda, lapack_float* w,
    lapack_float* work, lapack_int lwork );


/* Functions from Fortran */
// Adapted to use lapack_float instead of float

lapack_float hfdot(int n, lapack_float *sx, int incx, lapack_float *sy, int incy);

void hfgemm(char transa, char transb, int m, int n, int k, lapack_float alpha, const lapack_float *a, 
    int lda, const lapack_float *b, int ldb, lapack_float beta, lapack_float *c, int ldc);

void hfgemv(char trans, int m, int n, lapack_float alpha, lapack_float *a, int lda,
    lapack_float *x, lapack_int incx, lapack_float beta, lapack_float *y, lapack_int incy);

void hfger(int m, int n, lapack_float alpha, const lapack_float *x, int incx, 
    const lapack_float *y, int incy, lapack_float *a, int lda);

void hflacpy(char uplo, int m, int n, const lapack_float *a, int lda, lapack_float *b, int ldb);

void hflae2(lapack_float a, lapack_float b, lapack_float c, lapack_float *rt1, lapack_float *rt2);

void hflaev2(lapack_float a, lapack_float b, lapack_float c, lapack_float *rt1, lapack_float *rt2, lapack_float *cs1, lapack_float *sn1);

lapack_float hflanst(char norm, int n, const lapack_float *d, const lapack_float *e);

lapack_float hflansy(char norm, char uplo, int n, const lapack_float *a, int lda, lapack_float *work);

lapack_float hflapy2(lapack_float x, lapack_float y);

void hflarf1f(char side, int m, int n, lapack_float *v, int incv, lapack_float tau, lapack_float *C, int ldc, lapack_float *work);

void hflarf1l(char side, int m, int n, lapack_float *v, int incv, lapack_float tau, lapack_float *C, int ldc, lapack_float *work);

void hflarfb(char side, char trans, char direct, char storev, int m, int n, int k,
    lapack_float *v, int ldv, lapack_float *t, int ldt, lapack_float *c, int ldc, lapack_float *work, int ldwork);

void hflarfg(int n, lapack_float *alpha, lapack_float *x, lapack_int incx, lapack_float *tau);

void hflarft(char direct, char storev, int n, int k, lapack_float *v, int ldv, lapack_float *tau, lapack_float *t, int ldt);

void hflartg(lapack_float f, lapack_float g, lapack_float *c, lapack_float *s, lapack_float *r);

void hflascl(char TYPE, lapack_int KL, lapack_int KU, lapack_float CFROM, lapack_float CTO,
    lapack_int M, lapack_int N, lapack_float *A, lapack_int LDA, lapack_int *INFO);

void hflaset(char uplo, int m, int n, lapack_float alpha, lapack_float beta, lapack_float* a, int lda);

void hflasr(char side, char pivot, char direct, lapack_int m, lapack_int n,
    const lapack_float* c, const lapack_float* s, lapack_float* a, lapack_int lda);

void hflasrt(char id, lapack_int n, lapack_float *d, lapack_int *info);

void hflassq(int n, const lapack_float *x, int incx, lapack_float *scale, lapack_float *sumsq);

void hflatrd(char uplo, int n, int nb, lapack_float *a, int lda, lapack_float *e, lapack_float *tau, lapack_float *w, int ldw);

lapack_float hfnrm2(int n, lapack_float *x, int incx);

void hforg2l(lapack_int m, lapack_int n, lapack_int k, lapack_float *a, lapack_int lda,
    lapack_float *tau, lapack_float *work, lapack_int *info);

void hforg2r(int m, int n, int k, lapack_float *A, int lda, lapack_float *tau, lapack_float *work, int *info);

void hforgql(int m, int n, int k, lapack_float *a, int lda, lapack_float *tau, lapack_float *work, int lwork, lapack_int *info);

void hforgqr(int m, int n, int k, lapack_float *a, int lda, lapack_float *tau, lapack_float *work, int lwork, int *info);

void hforgtr(char uplo, int n, lapack_float *a, int lda, lapack_float *tau, lapack_float *work, lapack_int lwork, lapack_int *info);

void hfscal(int n, lapack_float sa, lapack_float *sx, int incx);

void hfsteqr(const char compz, int n, lapack_float *d, lapack_float *e, lapack_float *z, int ldz, lapack_float *work, int *info);

void hfsterf(lapack_int n, lapack_float *d, lapack_float *e, int *info);

void hfswap(int n, lapack_float *sx, int incx, lapack_float *sy, int incy);

void hfsymv(char uplo, lapack_int n, lapack_float alpha, lapack_float *A, lapack_int lda,
    lapack_float *x, lapack_int incx, lapack_float beta, lapack_float *y, lapack_int incy);

void hfsyr2(char uplo, lapack_int n, lapack_float alpha, const lapack_float *x, lapack_int incx,
    const lapack_float *y, lapack_int incy, lapack_float *a, lapack_int lda);

void hfsyr2k(char uplo, char trans, int n, int k, lapack_float alpha,
    lapack_float *A, int lda, lapack_float *B, int ldb, lapack_float beta,
    lapack_float *C, int ldc);

void hfsytd2(char uplo, lapack_int n, lapack_float *a, int lda, lapack_float *d, lapack_float *e, lapack_float *tau, lapack_int *info);

void hfsytrd(char uplo, lapack_int n, lapack_float *a, lapack_int lda, lapack_float *d,
    lapack_float *e, lapack_float *tau, lapack_float *work, lapack_int lwork, lapack_int *info);

void hftrmm(const char side, const char uplo, const char transa, const char diag,
    const int m, const int n, const lapack_float alpha, const lapack_float *a, const int lda,
    lapack_float *b, const int ldb);






void hfcopy(int n, const lapack_float *sx, int incx, lapack_float *sy, int incy);

lapack_float hflamch_half_precision(const char cmach);

lapack_float hfroundup_lwork(int lwork);

int ieeeck_reimpl_half_precision(int ispec, lapack_float *zero, lapack_float *one);

int ilaenv_reimpl_half_precision(int ispec, char* name, char* opts, int n1, int n2, int n3, int n4);

int ilaslc_reimpl_half_precision(int m, int n, lapack_float *a, int lda);

int ilaslr_reimpl_half_precision(int m, int n, lapack_float *a, int lda);



/* No modified functions */
// Functions that do the same as the original LAPACK functions but implemented in C

int iparmq_reimpl(int ispec, const char *name, const char *opts, int n, int ilo, int ihi, int lwork);

/* The same functions */


#endif /* LAPACKE_UTILS_REIMPL_H */
