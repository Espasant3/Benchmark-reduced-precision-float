/*
 * Adaptado de LAPACK (netlib.org/lapack) para media precisión
 * 
 * Copyright original:
 *   Copyright (c) 1992-2025 The University of Tennessee and The University
 *                        of Tennessee Research Foundation. All rights reserved.
 *   Copyright (c) 2000-2025 The University of California Berkeley. All rights reserved.
 *   Copyright (c) 2006-2025 The University of Colorado Denver. All rights reserved.
 * 
 * Modificaciones (c) 2025 Eloi Barcón Piñeiro
 * 
 * Licencia: BSD modificada (ver ../../../../LICENSE_LAPACK)
 */

#include "lapacke_utils_reimpl.h"

void hfsymv(char uplo, lapack_int n, lapack_float alpha, lapack_float *A, lapack_int lda,
           lapack_float *x, lapack_int incx, lapack_float beta, lapack_float *y, lapack_int incy) {
    
    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;

    lapack_int info = 0;
    if (!lsame_reimpl(uplo, 'U') && !lsame_reimpl(uplo, 'L')) {
        info = 1;
    } else if (n < 0) {
        info = 2;
    } else if (lda < MAX(1, n)) {
        info = 5;
    } else if (incx == 0) {
        info = 7;
    } else if (incy == 0) {
        info = 10;
    }
    if (info != 0) {
        c_xerbla("ssymv ", info);
        return;
    }

    if (n == 0 || (alpha == ZERO && beta == ONE)) {
        return;
    }

    // Escalado de Y con beta (operaciones directas en lapack_float)
    if (beta != ONE) {
        if (incy == 1) {
            if (beta == ZERO) {
                for (lapack_int i = 0; i < n; i++) {
                    y[i] = ZERO;
                }
            } else {
                for (lapack_int i = 0; i < n; i++) {
                    y[i] *= beta;  // Escalado directo
                }
            }
        } else {
            lapack_int iy = (incy > 0) ? 0 : ((n - 1) * ABS(incy));
            if (beta == ZERO) {
                for (lapack_int i = 0; i < n; i++) {
                    y[iy] = ZERO;
                    iy += incy;
                }
            } else {
                for (lapack_int i = 0; i < n; i++) {
                    y[iy] *= beta;  // Escalado directo
                    iy += incy;
                }
            }
        }
    }

    if (alpha == ZERO) {
        return;
    }

    // Cálculo principal sin conversiones intermedias
    if (lsame_reimpl(uplo, 'U')) {
        if (incx == 1 && incy == 1) {
            for (lapack_int j = 0; j < n; j++) {
                lapack_float xj = x[j];                
                lapack_float temp1 = alpha * xj;
                lapack_float temp2 = ZERO;

                // Parte triangular superior
                for (lapack_int i = 0; i < j; i++) {
                    lapack_float A_ij = A[i + j * lda];
                    y[i] += temp1 * A_ij;
                    temp2 += A_ij * x[i];
                }

                // Diagonal
                y[j] += temp1 * A[j + j * lda] + alpha * temp2;
            }
        } else {
            lapack_int kx = (incx > 0) ? 0 : ((n - 1) * ABS(incx));
            lapack_int ky = (incy > 0) ? 0 : ((n - 1) * ABS(incy));
            lapack_int jx = kx;
            lapack_int jy = ky;

            for (lapack_int j = 0; j < n; j++) {
                lapack_float xj = x[jx];
                lapack_float temp1 = alpha * xj;
                lapack_float temp2 = ZERO;
                lapack_int ix = kx;
                lapack_int iy = ky;

                for (lapack_int i = 0; i < j; i++) {
                    lapack_float A_ij = A[i + j * lda];
                    y[iy] += temp1 * A_ij;
                    temp2 += A_ij * x[ix];
                    ix += incx;
                    iy += incy;
                }

                y[jy] += temp1 * A[j + j * lda] + alpha * temp2;
                jx += incx;
                jy += incy;
            }
        }
    } else {
        if (incx == 1 && incy == 1) {
            for (lapack_int j = 0; j < n; j++) {
                lapack_float temp1 = alpha * x[j];      // Conversión eliminada
                lapack_float temp2 = ZERO;

                // Diagonal
                y[j] += temp1 * A[j + j * lda];

                // Parte triangular inferior
                for (lapack_int i = j + 1; i < n; i++) {
                    lapack_float A_ij = A[i + j * lda];
                    y[i] += temp1 * A_ij;
                    temp2 += A_ij * x[i];
                }

                y[j] += alpha * temp2;
            }
        } else {
            lapack_int kx = (incx > 0) ? 0 : ((n - 1) * ABS(incx));
            lapack_int ky = (incy > 0) ? 0 : ((n - 1) * ABS(incy));
            lapack_int jx = kx;
            lapack_int jy = ky;

            for (lapack_int j = 0; j < n; j++) {
                lapack_float temp1 = alpha * x[jx];
                lapack_float temp2 = ZERO;

                // Diagonal
                y[jy] += temp1 * A[j + j * lda];

                lapack_int ix = jx + incx;
                lapack_int iy_val = jy + incy;

                // Parte triangular inferior
                for (lapack_int i = j + 1; i < n; i++) {
                    lapack_float A_ij = A[i + j * lda];
                    y[iy_val] += temp1 * A_ij;
                    temp2 += A_ij * x[ix];
                    ix += incx;
                    iy_val += incy;
                }

                y[jy] += alpha * temp2;
                jx += incx;
                jy += incy;
            }
        }
    }
}