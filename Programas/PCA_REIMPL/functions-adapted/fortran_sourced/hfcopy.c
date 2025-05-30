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
 * \brief Copia un vector SX a un vector SY.
 * 
 * \param[in] n Número de elementos en los vectores.
 * \param[in] sx Array de origen (0-based).
 * \param[in] incx Incremento de índice para SX.
 * \param[out] sy Array destino (0-based).
 * \param[in] incy Incremento de índice para SY.
 * 
 * Esta función copia elementos del vector SX al vector SY, manejando
 * diferentes incrementos y optimización para incrementos unitarios.
 */

void hfcopy(int n, const lapack_float *sx, int incx, lapack_float *sy, int incy) {
    int i, m;
    int ix, iy;

    if (n <= 0) return;

    if (incx == 1 && incy == 1) {
        /* Optimización para incrementos unitarios */
        m = n % 7;
        for (i = 0; i < m; ++i) {
            sy[i] = sx[i];
        }
        if (n < 7) return;
        for (i = m; i < n; i += 7) {
            sy[i]     = sx[i];
            sy[i + 1] = sx[i + 1];
            sy[i + 2] = sx[i + 2];
            sy[i + 3] = sx[i + 3];
            sy[i + 4] = sx[i + 4];
            sy[i + 5] = sx[i + 5];
            sy[i + 6] = sx[i + 6];
        }
    } else {
        /* Caso general para incrementos arbitrarios */
        ix = (incx > 0) ? 0 : (n - 1) * -incx;
        iy = (incy > 0) ? 0 : (n - 1) * -incy;
        
        for (i = 0; i < n; ++i) {
            sy[iy] = sx[ix];
            ix += incx;
            iy += incy;
        }
    }
}