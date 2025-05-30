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

void hfsyr2(char uplo, lapack_int n, lapack_float alpha, const lapack_float *x, lapack_int incx,
           const lapack_float *y, lapack_int incy, lapack_float *a, lapack_int lda) {

    const lapack_float ZERO = (lapack_float)0.0;

    if (!lsame_reimpl(uplo, 'U') && !lsame_reimpl(uplo, 'L')) {
        c_xerbla("HFSYR2 ", 1);
        return;
    }
    if (n < 0) {
        c_xerbla("HFSYR2 ", 2);
        return;
    }
    if (incx == 0) {
        c_xerbla("HFSYR2 ", 5);
        return;
    }
    if (incy == 0) {
        c_xerbla("HFSYR2 ", 7);
        return;
    }
    if (lda < MAX(1, n)) {
        c_xerbla("HFSYR2 ", 9);
        return;
    }

    if (n == 0 || alpha == ZERO) {
        return;
    }

    int non_unit_inc = (incx != 1) || (incy != 1);
    lapack_int kx = 0, ky = 0, jx = 0, jy = 0;

    if (non_unit_inc) {
        kx = (incx > 0) ? 0 : (n - 1) * ABS(incx);
        ky = (incy > 0) ? 0 : (n - 1) * ABS(incy);
        jx = kx;
        jy = ky;
    }

    if (lsame_reimpl(uplo, 'U')) {
        if (!non_unit_inc) {
            for (lapack_int j = 0; j < n; ++j) {
                lapack_float xj = x[j], yj = y[j];
                if (xj == ZERO && yj == ZERO) continue;
                lapack_float tmp1 = yj * alpha;
                lapack_float tmp2 = xj * alpha;
                for (lapack_int i = 0; i <= j; ++i) {
                    a[i + j * lda] += x[i] * tmp1 + y[i] * tmp2;
                }
            }
        } else {
            for (lapack_int j = 0; j < n; ++j) {
                lapack_float x_val = x[jx], y_val = y[jy];
                if (x_val != ZERO || y_val != ZERO) {
                    lapack_float tmp1 = y_val * alpha;
                    lapack_float tmp2 = x_val * alpha;
                    lapack_int ix = kx, iy = ky;
                    for (lapack_int i = 0; i <= j; ++i) {
                        a[i + j * lda] += x[ix] * tmp1 + y[iy] * tmp2;
                        ix += incx;
                        iy += incy;
                    }
                }
                jx += incx;
                jy += incy;
            }
        }
    } else {
        if (!non_unit_inc) {
            for (lapack_int j = 0; j < n; ++j) {
                lapack_float xj = x[j], yj = y[j];
                if (xj == ZERO && yj == ZERO) continue;
                lapack_float tmp1 = yj * alpha;
                lapack_float tmp2 = xj * alpha;
                for (lapack_int i = j; i < n; ++i) {
                    a[i + j * lda] += x[i] * tmp1 + y[i] * tmp2;
                }
            }
        } else {
            for (lapack_int j = 0; j < n; ++j) {
                lapack_float x_val = x[jx], y_val = y[jy];
                if (x_val != ZERO || y_val != ZERO) {
                    lapack_float tmp1 = y_val * alpha;
                    lapack_float tmp2 = x_val * alpha;
                    lapack_int ix = jx, iy = jy;
                    for (lapack_int i = j; i < n; ++i) {
                        a[i + j * lda] += x[ix] * tmp1 + y[iy] * tmp2;
                        ix += incx;
                        iy += incy;
                    }
                }
                jx += incx;
                jy += incy;
            }
        }
    }
}