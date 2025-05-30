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

void hforg2l(lapack_int m, lapack_int n, lapack_int k, lapack_float *a, lapack_int lda,
            lapack_float *tau, lapack_float *work, lapack_int *info) {
    
    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;

    *info = 0;
    if (m < 0) {
        *info = -1;
    } else if (n < 0 || n > m) {
        *info = -2;
    } else if (k < 0 || k > n) {
        *info = -3;
    } else if (lda < MAX(1, m)) {
        *info = -5;
    }
    if (*info != 0) {
        LAPACKE_xerbla("hforg2l", -(*info));
        return;
    }

    if (n <= 0) return;

    for(int j = 0; j < n - k - 1; j++){
        for(int l = 0; l < m - 1; l++){
            a[l + j * lda] = ZERO;
        }
        a[m - n + j + j * lda] = ONE;
    }
    
    for(int i = 1; i <= k; i++){
        int ii = n - k + i; 
        a[m - n + (ii - 1) + (ii - 1) * lda] = ONE;

        hflarf1l('L', m - n + ii, ii - 1, &a[(ii - 1) * lda], 1, tau[i - 1],
                        a, lda, work);
        hfscal(m - n + ii - 1, -tau[i - 1], &a[(ii - 1) * lda], 1);
        a[m - n + (ii - 1) + (ii - 1) * lda] = ONE - tau[i - 1];

        for(int l = m - n + ii; l < m; l++){
            a[l + (ii - 1) * lda] = ZERO;
        }
    }
}