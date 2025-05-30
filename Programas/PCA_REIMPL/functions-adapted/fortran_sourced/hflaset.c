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

/**
 * \file hflaset.c
 * \brief Inicializa una matriz en lapack_float con valores específicos
 */

/**
 * \brief Configura una matriz con valores constantes en lapack_float según el triángulo especificado
 * 
 * \details Versión lapack_float de LAPACK's SLASET. Inicializa:
 *          - Elementos fuera de la diagonal: alpha
 *          - Elementos diagonales: beta
 *          según la región especificada por uplo.
 * 
 * \param[in] uplo  Región a inicializar (case-insensitive):
 *                 - 'U': Triángulo superior estricto (sin diagonal)
 *                 - 'L': Triángulo inferior estricto (sin diagonal)
 *                 - Otro: Toda la matriz
 * \param[in] m     Número de filas de la matriz (m >= 0)
 * \param[in] n     Número de columnas de la matriz (n >= 0)
 * \param[in] alpha Valor lapack_float para elementos no diagonales
 * \param[in] beta  Valor lapack_float para elementos diagonales
 * \param[in,out] a Matriz lapack_float column-major (lda >= max(1,m))
 * \param[in] lda   Leading dimension de la matriz
 * 
 * \note
 * - La diagonal principal siempre se establece a beta (independiente de uplo)
 * - El orden column-major implica: a[i + j*lda] = fila i, columna j
 * - Para matrices cuadradas (m=n):
 *   - 'U' afecta elementos donde i < j
 *   - 'L' afecta elementos donde i > j
 * 
 * \warning
 * - No se valida la consistencia de lda vs m (comportamiento indefinido si lda < m)
 * - uplo no reconocido inicializa toda la matriz (incluyendo casos 'A' u otros)
 * - No maneja valores NaN/Inf en alpha/beta
 * 
 * \example
 * // Matriz 3x3 con diagonal 1 y triángulo superior 0.5
 * lapack_float A[9];
 * hflaset('U', 3, 3, 0.5F16, 1.0F16, A, 3);
 * // A = [[1, 0.5, 0.5],
 * //      [0,   1, 0.5],
 * //      [0,   0,   1]]
 * 
 * \example
 * // Matriz 2x4 con todos elementos 5 y diagonal 3
 * hflaset('G', 2, 4, 5.0F16, 3.0F16, B, 2);
 * // B = [[3, 5, 5, 5],
 * //      [5, 3, 5, 5]]
 * 
 * \see lsame_reimpl Para comparación case-insensitive de caracteres
 */

void hflaset(char uplo, int m, int n, lapack_float alpha, lapack_float beta, lapack_float* a, int lda) {
    int i, j;

    if (lsame_reimpl(uplo, 'U')) {
        // Parte triangular superior estricta
        for (j = 0; j < n; ++j) {
            for (i = 0; i < MIN(j, m); ++i) {
                a[i * lda + j] = alpha; // Row-major: fila i, columna j
            }
        }
    } else if (lsame_reimpl(uplo, 'L')) {
        // Parte triangular inferior estricta (column-major)
        for (j = 0; j < MIN(m, n); ++j) {
            for (i = j + 1; i < m; ++i) {
                a[i * lda + j] = alpha; // Row-major: fila i, columna j
            }
        }
    } else {
        // Toda la matriz
        for (j = 0; j < n; ++j) {
            for (i = 0; i < m; ++i) {
                a[i * lda + j] = alpha; // Row-major: fila i, columna j
            }
        }
    }

    // Diagonal principal
    int min_mn = MIN(m, n);
    for (i = 0; i < min_mn; ++i) {
        a[i * lda + i] = beta; // Row-major: fila i, columna i
    }
}