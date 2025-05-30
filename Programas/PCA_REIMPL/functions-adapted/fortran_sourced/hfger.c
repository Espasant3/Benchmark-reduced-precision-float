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

void hfger(int m, int n, lapack_float alpha, const lapack_float *x, int incx, 
         const lapack_float *y, int incy, lapack_float *a, int lda) {

    // Constantes
    const lapack_float ZERO = (lapack_float)0.0;

    int info = 0;
    if (m < 0) {
        info = 1;
    } else if (n < 0) {
        info = 2;
    } else if (incx == 0) {
        info = 5;
    } else if (incy == 0) {
        info = 7;
    } else if (lda < MAX(1, m)) {
        info = 9;
    }
    
    if (info != 0) {
        c_xerbla("HFGER", info);
        return;
    }

    // Cambiar constante 0.0f por conversión explícita
    if (m == 0 || n == 0 || alpha == ZERO) {
        return;
    }

    int jy = (incy > 0) ? 0 : (n - 1) * (-incy);

    if (incx == 1) {
        for (int j = 0; j < n; j++) {
            const lapack_float y_val = y[jy];
            if (y_val != ZERO) {
                const lapack_float temp = alpha * y_val;
                for (int i = 0; i < m; i++) {
                    a[i + j * lda] += x[i] * temp;
                }
            }
            jy += incy;
        }
    } else {
        const int kx = (incx > 0) ? 0 : (m - 1) * (-incx);
        
        for (int j = 0; j < n; j++) {
            const lapack_float y_val = y[jy];
            if (y_val != ZERO) {
                const lapack_float temp = alpha * y_val;
                int ix = kx;
                for (int i = 0; i < m; i++) {
                    a[i + j * lda] += x[ix] * temp;
                    ix += incx;
                }
            }
            jy += incy;
        }
    }
}