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
 * \file hforgtr.c
 * \brief Implementación de la generación de matriz Q ortogonal en lapack_float para matrices tridiagonales
 */

/**
 * \brief Genera la matriz Q ortogonal completa a partir de una factorización tridiagonal en lapack_float
 * 
 * \details Versión lapack_float de LAPACK's SORGTR. Genera la matriz Q ortogonal a partir de una descomposición
 *          tridiagonal simétrica computada por hsteqr. Implementa dos variantes según el triángulo almacenado.
 * 
 * \param[in] uplo  'U': Almacena matriz triangular superior
 *                  'L': Almacena matriz triangular inferior
 * \param[in] n     Orden de la matriz Q (n >= 0)
 * \param[in,out] a Matriz lapack_float column-major (lda, n):
 *                  - Entrada: Vectores de Householder de hsteqr
 *                  - Salida:  Matriz Q completa
 * \param[in] lda   Leading dimension de A (lda >= max(1,n))
 * \param[in] tau   Factores escalares lapack_float de los reflectores (tamaño n-1)
 * \param[out] work Workspace lapack_float (tamaño >= max(1,lwork))
 * \param[in] lwork Dimensión de work:
 *                  - lwork = -1: Consulta tamaño óptimo
 *                  - lwork >= max(1, n-1) para óptimo rendimiento
 * \param[out] info Código de retorno:
 *                  - 0: éxito
 *                  - <0: parámetro (-i) incorrecto
 *
 * \note
 * - Implementación específica para lapack_float (IEEE 754-2008 binary16)
 * - Diferencias clave vs versión double:
 *   + Uso de hfroundup_lwork para ajuste seguro de workspace
 *   + Optimizaciones para precisión reducida
 * - Algoritmo bifurcado según uplo:
 *   + Upper: Usa hforgql para factorización QL
 *   + Lower: Usa hforgqr para factorización QR
 * 
 * \warning
 * - Requiere que A contenga la descomposición tridiagonal simétrica de hsteqr
 * - Comportamiento indefinido si uplo no es 'U'/'L' (sin validación case-sensitive)
 * - lwork insuficiente causa error -7 sin manejo de recuperación
 *
 * \par Algoritmo:
 * 1. Validación de parámetros y cálculo de workspace óptimo
 * 2. Reorganización de la matriz según uplo:
 *    \code
 *    Upper: Desplaza columnas izquierda y limpia última columna
 *    Lower: Desplaza columnas derecha y limpia primera columna
 *    \endcode
 * 3. Generación de Q mediante:
 *    - hforgql para uplo='U'
 *    - hforgqr para uplo='L'
 *
 * \example
 * int n = 4, lwork;
 * lapack_float a[16], tau[3];
 * lapack_int info;
 * 
 * // Factorización tridiagonal con hsteqr...
 * lwork = -1;
 * hforgtr('U', n, a, n, tau, work, lwork, &info); // Consulta tamaño
 * lwork = (int)work[0];
 * lapack_float* work = malloc(lwork * sizeof(lapack_float));
 * hforgtr('U', n, a, n, tau, work, lwork, &info);
 *
 * \see hsteqr     Para la factorización tridiagonal inicial
 * \see hforgql    Para generación QL en lapack_float
 * \see hforgqr    Para generación QR en lapack_float
 * \see ilaenv_reimpl_half_precision  Para determinación de parámetros de bloqueo
 * \see hfroundup_lwork  Para ajuste preciso de workspace
 * \see IEEE754_FP16  Para constantes numéricas de media precisión
 */

void hforgtr(char uplo, int n, lapack_float *a, int lda, lapack_float *tau, lapack_float *work, lapack_int lwork, lapack_int *info) {
    int upper, i, j, lquery, nb, lwkopt, iinfo;
    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;

    *info = 0;
    lquery = (lwork == -1);
    upper = lsame_reimpl(uplo, 'U');

    // Validación de parámetros
    if (!upper && !lsame_reimpl(uplo, 'L')) {
        *info = -1;
    } else if (n < 0) {
        *info = -2;
    } else if (lda < MAX(1, n)) {
        *info = -4;
    } else if (lwork < MAX(1, n-1) && !lquery) {
        *info = -7;
    }

    // Cálculo de tamaño óptimo de WORK
    if (*info == 0) {
        if (upper) {
            nb = ilaenv_reimpl_half_precision(1, "SORGQL", " ", n-1, n-1, n-1, -1);
        } else {
            nb = ilaenv_reimpl_half_precision(1, "SORGQR", " ", n-1, n-1, n-1, -1);
        }
        lwkopt = MAX(1, n-1) * nb;
        work[0] = hfroundup_lwork(lwkopt);
    }

    if (*info != 0) {
        LAPACKE_xerbla("HFORGTR", -(*info));
        return;
    } else if (lquery) {
        return;
    }

    if (n == 0) {
        work[0] = ONE;
        return;
    }

    if (upper) {
        // ---- Caso UPPER (UPLO = 'U') ----
        for (j = 0; j < n-1; j++) {
            for (i = 0; i < j-1; i++) {
                a[i + j*lda] = a[i + (j+1)*lda];
            }
            a[(n-1) + j*lda] = ZERO; 
        }

        for (i = 0; i < n-1; i++) {
            a[i + (n-1)*lda] = ZERO;
        }
        a[(n-1) + (n-1)*lda] = ONE;

        hforgql(n-1, n-1, n-1, a, lda, tau, work, lwork, &iinfo);

    } else {
        // ---- Caso LOWER (UPLO = 'L') ----
        for (j = n-1; j >= 1; j--) {
            a[j*lda] = ZERO; 
            for (i = j+1; i < n; i++) {
                a[i + j*lda] = a[i + (j-1)*lda];
            }
        }

        a[0] = ONE;
        for (i = 1; i < n; i++) {
            a[i] = ZERO; 
        }

        if (n > 1) {
            hforgqr(n-1, n-1, n-1, &a[1 + 1*lda], lda, tau, work, lwork, &iinfo);
        }
    }

    work[0] = hfroundup_lwork(lwkopt);
}