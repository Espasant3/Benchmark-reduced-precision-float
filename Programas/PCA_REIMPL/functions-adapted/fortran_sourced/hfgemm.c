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

void hfgemm(char transa, char transb, int m, int n, int k, lapack_float alpha, const lapack_float *a, 
            int lda, const lapack_float *b, int ldb, lapack_float beta, lapack_float *c, int ldc) {

    // Constantes

    const lapack_float ZERO = (lapack_float)0.0;
    const lapack_float ONE = (lapack_float)1.0;

    lapack_logical nota, notb;
    int nrowa, nrowb;
    int info = 0;
    int i, j, l;
    lapack_float temp;

    nota = lsame_reimpl(transa, 'N');
    notb = lsame_reimpl(transb, 'N');

    nrowa = nota ? m : k;
    nrowb = notb ? k : n;

    // Validación de parámetros (igual que antes)
    if ((!nota) && !lsame_reimpl(transa, 'T') && !lsame_reimpl(transa, 'C')) {
        info = 1;
    } else if ((!notb) && !lsame_reimpl(transb, 'T') && !lsame_reimpl(transb, 'C')) {
        info = 2;
    } else if (m < 0) {
        info = 3;
    } else if (n < 0) {
        info = 4;
    } else if (k < 0) {
        info = 5;
    } else if (lda < MAX(1, nrowa)) {
        info = 8;
    } else if (ldb < MAX(1, nrowb)) {
        info = 10;
    } else if (ldc < MAX(1, m)) {
        info = 13;
    }

    if (info != 0) {
        c_xerbla("HFGEMM ", info);
        return;
    }

    // Retorno rápido (igual que antes)
    if (m == 0 || n == 0 || ((alpha == ZERO || k == 0) && beta == ONE)) {
        return;
    }

    // Manejo de alpha == ZERO (igual que antes)
    if (alpha == ZERO) {
        if (beta == ZERO) {
            for (j = 0; j < n; j++) {
                for (i = 0; i < m; i++) {
                    c[i + j * ldc] = ZERO;
                }
            }
        } else {
            for (j = 0; j < n; j++) {
                for (i = 0; i < m; i++) {
                    c[i + j * ldc] *= beta;
                }
            }
        }
        return;
    }

    // Casos de multiplicación (lógica idéntica)
    if (notb) {
        if (nota) {
            // C = alpha*A*B + beta*C
            for (j = 0; j < n; j++) {
                if (beta == ZERO) {
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] = ZERO;
                    }
                } else if (beta != ONE) {
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] *= beta;
                    }
                }
                for (l = 0; l < k; l++) {
                    temp = alpha * b[l + j * ldb];
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] += temp * a[i + l * lda];
                    }
                }
            }
        } else {
            // C = alpha*A^T*B + beta*C
            for (j = 0; j < n; j++) {
                for (i = 0; i < m; i++) {
                    temp = ZERO;
                    for (l = 0; l < k; l++) {
                        temp += a[l + i * lda] * b[l + j * ldb];
                    }
                    if (beta == ZERO) {
                        c[i + j * ldc] = alpha * temp;
                    } else {
                        c[i + j * ldc] = alpha * temp + beta * c[i + j * ldc];
                    }
                }
            }
        }
    } else {
        if (nota) {
            // C = alpha*A*B^T + beta*C
            for (j = 0; j < n; j++) {
                if (beta == ZERO) {
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] = ZERO;
                    }
                } else if (beta != ONE) {
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] *= beta;
                    }
                }
                for (l = 0; l < k; l++) {
                    temp = alpha * b[j + l * ldb];
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] += temp * a[i + l * lda];
                    }
                }
            }
        } else {
            // C = alpha*A^T*B^T + beta*C
            for (j = 0; j < n; j++) {
                for (i = 0; i < m; i++) {
                    temp = ZERO;
                    for (l = 0; l < k; l++) {
                        temp += a[l + i * lda] * b[j + l * ldb];
                    }
                    if (beta == ZERO) {
                        c[i + j * ldc] = alpha * temp;
                    } else {
                        c[i + j * ldc] = alpha * temp + beta * c[i + j * ldc];
                    }
                }
            }
        }
    }
}