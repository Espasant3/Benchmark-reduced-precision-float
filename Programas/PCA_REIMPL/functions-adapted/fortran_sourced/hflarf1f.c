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

void hflarf1f(char side, int m, int n, lapack_float *v, int incv, lapack_float tau, lapack_float *C, int ldc, lapack_float *work) {
    
    // Constantes
    lapack_float ZERO = (lapack_float)0.0;
    lapack_float ONE = (lapack_float)1.0;
    
    int applyleft, lastv, lastc;

    /* Verificar lado de la operación */
    applyleft = lsame_reimpl(side, 'L');

    lastv = 1;
    lastc = 0;
    if (tau != ZERO) {
        lastv = applyleft ? m : n;
        int i;
        
        if (incv > 0) {
            i = (lastv - 1) * incv;  // Último elemento posible
        } else {
            i = 0;  // Manejo de INCV negativo
        }
        
        // Detenerse cuando LASTV > 1 como en Fortran
        while(lastv > 1 && v[i] == ZERO){
            lastv--;
            i -= incv;
        }

        if(applyleft){
            lastc = ilaslc_reimpl_half_precision(lastv, n, C, ldc) + 1; // +1 para incluir la fila 0
        }else{
            lastc = ilaslr_reimpl_half_precision(m, lastv, C, ldc) + 1; // +1 para incluir la columna 0
            
        }

    }
    if (lastc == 0) return;

    if (applyleft) {
        if (lastv == 1) {
            hfscal(lastc, ONE - tau, C, ldc);
        } else {
            /* w = C(2:lastv,1:lastc)^T * v(2:lastv) */
            hfgemv('T', lastv-1, lastc, ONE, &C[1], ldc,
                   &v[incv], incv, ZERO, work, 1);

            /* w += v[0] * C[0] */
            hfaxpy(lastc, ONE, C, ldc, work, 1);

            /* C = C - tau * v * w^T */
            hfaxpy(lastc, -tau, work, 1, C, ldc);

            hfger(lastv-1, lastc, -tau, &v[incv], incv,
                  work, 1, &C[1], ldc);
        }
    } else {
        if (lastv == 1) {
            hfscal(lastc, ONE - tau, C, 1);
        } else {
            /* w = C(1:lastc,2:lastv) * v(2:lastv) */
            hfgemv('N', lastc, lastv-1, ONE, &C[ldc], ldc,
                   &v[incv], incv, ZERO, work, 1);

            /* w += v[0] * C[0] */
            hfaxpy(lastc, 1, C, 1, work, 1);

            /* C = C - tau * w * v^T */
            hfaxpy(lastc, -tau, work, 1, C, 1);

            hfger(lastc, lastv-1, -tau, work, 1,
                  &v[incv], incv, &C[ldc], ldc);
        }
    }
}