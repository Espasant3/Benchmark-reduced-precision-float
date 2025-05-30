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

void hflassq(int n, const lapack_float *x, int incx, lapack_float *scale, lapack_float *sumsq) {
    if (n <= 0 || LAPACK_HFISNAN(*scale) || LAPACK_HFISNAN(*sumsq) || *sumsq < 0.0F16) return;

    float scale_f = (float)(*scale);
    float sumsq_f = (float)(*sumsq);
    int ix = (incx >= 0) ? 0 : (1 - n) * incx;

    for (int i = 0; i < n; ++i) {
        float xi = fabsf((float)x[ix]);
        if (xi != 0.0f) {
            if (scale_f < xi) {
                float ratio = scale_f / xi;
                sumsq_f = 1.0f + sumsq_f * ratio * ratio;
                scale_f = xi;
            } else {
                float ratio = xi / scale_f;
                sumsq_f += ratio * ratio;
            }
            // Saturación intermedia para evitar overflow
            #ifndef USE_BF16
            if (sumsq_f > FP16_MAX) {
                sumsq_f = FP16_MAX;
                scale_f = 1.0f;
                break;  // Salir del bucle si ya se alcanzó el máximo
            }
            #else
            if (sumsq_f > BF16_MAX) {
                sumsq_f = BF16_MAX;
                scale_f = 1.0f;
                break;  // Salir del bucle si ya se alcanzó el máximo
            }
            #endif
        }
        ix += incx;
    }

    // Saturación final
    #ifndef USE_BF16
    if (sumsq_f > FP16_MAX) {
        sumsq_f = FP16_MAX;
        scale_f = 1.0f;
    }
    #else
    if (sumsq_f > BF16_MAX) {
        sumsq_f = BF16_MAX;
        scale_f = 1.0f;
    }
    #endif

    *scale = (lapack_float)scale_f;
    *sumsq = (lapack_float)sumsq_f;
}