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
 * \brief Calcula el producto punto (dot product) de dos vectores de tipo lapack_float.
 * 
 * \param[in] n Número de elementos en los vectores.
 * \param[in] sx Array del primer vector (0-based).
 * \param[in] incx Incremento de índice para el vector SX.
 * \param[in] sy Array del segundo vector (0-based).
 * \param[in] incy Incremento de índice para el vector SY.
 * \return lapack_float Resultado del producto punto acumulado.
 * 
 * Esta función calcula el producto punto entre los vectores SX y SY. Optimiza
 * el cálculo para incrementos unitarios procesando bloques de 5 elementos,
 * y maneja incrementos arbitrarios ajustando los índices según los incrementos
 * especificados. Si n <= 0, retorna 0.0.
 */
lapack_float hfdot(int n, lapack_float *sx, int incx, lapack_float *sy, int incy) {
    lapack_float stemp = (lapack_float)0.0;
    int i, ix, iy, m;

    if (n <= 0) {
        return stemp;
    }

    if (incx == 1 && incy == 1) {
        // Manejo óptimo para incrementos unitarios
        m = n % 5;
        for (i = 0; i < m; ++i) {
            stemp += sx[i] * sy[i];
        }
        if (n < 5) {
            return stemp;
        }
        // Procesamiento en bloques de 5
        for (i = m; i < n; i += 5) {
            stemp += sx[i] * sy[i] 
                   + sx[i + 1] * sy[i + 1]
                   + sx[i + 2] * sy[i + 2]
                   + sx[i + 3] * sy[i + 3]
                   + sx[i + 4] * sy[i + 4];
        }
    } else {
        // Caso general para incrementos arbitrarios
        ix = (incx < 0) ? ((-n + 1) * incx) : 0;
        iy = (incy < 0) ? ((-n + 1) * incy) : 0;

        for (i = 0; i < n; ++i) {
            stemp += sx[ix] * sy[iy];
            ix += incx;
            iy += incy;
        }
    }

    return stemp;
}