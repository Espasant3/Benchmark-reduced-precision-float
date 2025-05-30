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

void hflacpy(char uplo, int m, int n, const lapack_float *a, int lda, lapack_float *b, int ldb) {
    int i, j;
    
    if (lsame_reimpl(uplo, 'U')) {
        for (j = 0; j < n; ++j) {
            const int max_i = (j + 1 < m) ? j + 1 : m;
            for (i = 0; i < max_i; ++i) {
                b[i + j * ldb] = a[i + j * lda];
            }
        }
    } else if (lsame_reimpl(uplo, 'L')) {
        for (j = 0; j < n; ++j) {
            const int start_i = (j < m) ? j : m;
            for (i = start_i; i < m; ++i) {
                b[i + j * ldb] = a[i + j * lda];
            }
        }
    } else {
        for (j = 0; j < n; ++j) {
            for (i = 0; i < m; ++i) {
                b[i + j * ldb] = a[i + j * lda];
            }
        }
    }
}