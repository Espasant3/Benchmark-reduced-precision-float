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
 * \file hforgql.c
 * \brief Generación de la matriz Q a partir de una factorización QL en precisión media
 */

/**
 * \brief Genera la matriz Q completa de una factorización QL en precisión media
 * 
 * \details Versión FP16 de LAPACK's SORGQL. Genera la matriz ortogonal Q a partir
 *          de una factorización QL obtenida por hgeqlf. La matriz Q se define
 *          como producto de reflectores elementales de Householder.
 * 
 * \param[in] m     Número de filas de la matriz A (m >= 0)
 * \param[in] n     Número de columnas de la matriz A (0 <= n <= m)
 * \param[in] k     Número de reflectores de Householder (0 <= k <= n)
 * \param[in,out] a Matriz de entrada/salida en formato column-major (lda x n)
 *                  - Entrada: Vectores de Householder desde hgeqlf
 *                  - Salida:  Matriz Q completa
 * \param[in] lda   Leading dimension de A (lda >= max(1,m))
 * \param[in] tau   Factores escalares de los reflectores (tamaño k)
 * \param[out] work Array de trabajo (tamaño max(1,lwork))
 * \param[in] lwork Dimensión del workspace (lwork >= max(1,n) para óptimo)
 * \param[out] info Código de salida:
 *                  - 0:  éxito
 *                  - <0: parámetro i tenía valor ilegal
 * 
 * \note
 * - Adaptación directa del código Fortran usando lapack_float
 * - Usa bloqueo dinámico con tamaño de bloque determinado por ilaenv_reimpl_half_precision
 * - Si lwork = -1, calcula tamaño óptimo y retorna en work[0]
 * 
 * \warning
 * - La matriz A debe contener los vectores de Householder de hgeqlf
 * - lda debe satisfacer lda >= m para evitar comportamiento indefinido
 * - No realiza comprobaciones de NaN/Inf en la entrada
 * 
 * \par Algoritmo:
 * 1. Validación de parámetros
 * 2. Cálculo de tamaño de bloque óptimo (nb)
 * 3. Ejecución en dos fases:
 *    - Fase 1: Código no bloqueado para submatriz inicial
 *    - Fase 2: Código bloqueado con actualizaciones mediante hflarfb
 * 
 * 
 * \example
 * int m = 4, n = 3, lwork;
 * lapack_float a[12], tau[3];
 * lapack_int info;
 * 
 * // Factorización QL con hgeqlf...
 * 
 * // Generar Q completa
 * lwork = -1;
 * hforgql(m, n, n, a, 4, tau, work, lwork, &info); // Consulta tamaño
 * lwork = (int)work[0];
 * lapack_float* work = malloc(lwork * sizeof(lapack_float));
 * hforgql(m, n, n, a, 4, tau, work, lwork, &info);
 * 
 * \see hgeqlf Para la factorización QL inicial
 * \see hflarfb Para aplicación de reflectores bloqueados
 * \see ilaenv_reimpl_half_precision Para determinar parámetros de bloqueo
 * \see hfroundup_lwork Para ajuste de workspace en FP16
 */

void hforgql(int m, int n, int k, lapack_float *a, int lda, lapack_float *tau, lapack_float *work, int lwork, lapack_int *info) {
    // Constantes y variables locales
    const int c__1 = 1;
    const int c_n1 = -1;
    const int c__3 = 3;
    const int c__2 = 2;
    const lapack_float ZERO = (lapack_float) 0.0;
    
    *info = 0;
    int lquery = (lwork == -1);

    // Validación de parámetros de entrada
    if (m < 0) {
        *info = -1;
    } else if (n < 0 || n > m) {
        *info = -2;
    } else if (k < 0 || k > n) {
        *info = -3;
    } else if (lda < MAX(1, m)) {
        *info = -5;
    }

    int lwkopt;
    if (*info == 0) {
        if (n == 0) {
            lwkopt = 1;
        } else {
            int nb = ilaenv_reimpl_half_precision(c__1, "SORGQL", " ", m, n, k, c_n1);
            lwkopt = n * nb;
        }
        work[0] = hfroundup_lwork(lwkopt);
        
        if (lwork < MAX(1, n) && !lquery) {
            *info = -8;
        }
    }

    if (*info != 0) {
        LAPACKE_xerbla("HFORGQL", *info);
        return;
    } else if (lquery) {
        return;
    }

    if (n <= 0) return;

    // Parámetros para bloqueo
    int nbmin = 2;
    int nx = 0;
    int iws = n;
    int nb = ilaenv_reimpl_half_precision(c__1, "SORGQL", " ", m, n, k, c_n1);
    
    if (nb > 1 && nb < k) {
        nx = MAX(0, ilaenv_reimpl_half_precision(c__3, "SORGQL", " ", m, n, k, c_n1));
        if (nx < k) {
            int ldwork = n;
            iws = ldwork * nb;
            if (lwork < iws) {
                nb = lwork / ldwork;
                nbmin = MAX(2, ilaenv_reimpl_half_precision(c__2, "SORGQL", " ", m, n, k, c_n1));
            }
        }
    }

    int kk;
    if (nb >= nbmin && nb < k && nx < k) {
        kk = MIN(k, ((k - nx + nb - 1) / nb) * nb);
        // Poner a cero las columnas 1:n-kk
        for (int j = 0; j < n - kk; ++j) {
            for (int i = m - kk; i < m; ++i) {
                a[i + j * lda] = ZERO;
            }
        }
    } else {
        kk = 0;
    }

    // Primer bloque con código no bloqueado
    if (m - kk > 0 && n - kk > 0) {
        int m_kk = m - kk;
        int n_kk = n - kk;
        int k_kk = k - kk;
        hforg2l(m_kk, n_kk, k_kk, a, lda, tau, work, info);
    }

    // Código bloqueado
    if (kk > 0) {
        for (int i = k - kk; i < k; i += nb) {
            int ib = MIN(nb, k - i);
            int start_col = n - k + i;
            
            if (n - k + i > 0) {
                // Formar factor triangular T
                int len = m - k + i + ib;
                int ldwork = ib;
                lapack_float *a_col = &a[start_col * lda];
                
                hflarft('B', 'C', len, ib, a_col, lda, &tau[i], work, ldwork);

                // Aplicar transformación Householder
                int cols = start_col;
                hflarfb('L', 'N', 'B', 'C', len, cols, ib,
                        a_col, lda, work, ldwork, 
                        a, lda, &work[ib], ldwork);
            }

            // Generar vectores de Householder
            int len = m - k + i + ib;
            lapack_float *current_a = &a[start_col * lda];
            hforg2l(len, ib, ib, current_a, lda, &tau[i], work, info);

            // Poner a cero las filas inferiores
            for (int j = start_col; j < start_col + ib; ++j) {
                for (int l = m - k + i + ib; l < m; ++l) {
                    a[l + j * lda] = ZERO;
                }
            }
        }
    }

    work[0] = hfroundup_lwork(iws);
}