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

// Main function to compute matrix norms for real symmetric matrices
lapack_float hflansy(char norm, char uplo, int n, const lapack_float *a, int lda, lapack_float *work) {
    
    // Constantes
    const lapack_float ZERO = (lapack_float)0.0;
    const lapack_float ONE = (lapack_float)1.0;
    
    lapack_float value = ZERO;

    if (n == 0) value = ZERO;

    if (lsame_reimpl(norm, 'M')) {
        // Maximum absolute value
        if (lsame_reimpl(uplo, 'U')) {
            for (int j = 0; j < n; ++j) {
                for (int i = 0; i <= j; ++i) {
                    lapack_float current = ABS_half_precision(a[i + j * lda]);
                    if (current > value || LAPACK_HFISNAN(current)) value = current;
                }
            }
        } else {
            for (int j = 0; j < n; ++j) {
                for (int i = j; i < n; ++i) {
                    lapack_float current = ABS_half_precision(a[i + j * lda]);
                    if (current > value || LAPACK_HFISNAN(current)) value = current;
                }
            }
        }
    } else if (lsame_reimpl(norm, 'I') || lsame_reimpl(norm, 'O') || norm == '1') {
        // Infinity norm or 1-norm
        value = ZERO;
        if (lsame_reimpl(uplo, 'U')) {
            for (int i = 0; i < n; ++i) work[i] = ZERO;
            for (int j = 0; j < n; ++j) {
                lapack_float sum = ZERO;
                for (int i = 0; i < j; ++i) {
                    lapack_float absa = ABS_half_precision(a[i + j * lda]);
                    sum += absa;
                    work[i] += absa;
                }
                sum += ABS_half_precision(a[j + j * lda]);
                work[j] += sum;
            }
            for (int i = 0; i < n; ++i) {
                lapack_float current = work[i];
                if (current > value || LAPACK_HFISNAN(current)) value = current;
            }
        } else {
            for (int i = 0; i < n; ++i) work[i] = ZERO;
            for (int j = 0; j < n; ++j) {
                lapack_float sum = work[j] + ABS_half_precision(a[j + j * lda]);
                for (int i = j + 1; i < n; ++i) {
                    lapack_float absa = ABS_half_precision(a[i + j * lda]);
                    sum += absa;
                    work[i] += absa;
                }
                if (sum > value || LAPACK_HFISNAN(sum)) value = sum;
            }
        }
    } else if (lsame_reimpl(norm, 'F') || lsame_reimpl(norm, 'E')) {
        lapack_float scale = ZERO;
        lapack_float sum = ONE;
        bool has_nan = false;

        // Verificar NaN en todos los elementos relevantes de la matriz
        // Dentro del bloque que verifica NaN en hflansy:
        if (lsame_reimpl(uplo, 'U')) {
            for (int j = 0; j < n; ++j) {
                for (int i = 0; i <= j; ++i) {
                    lapack_float elem = a[i + j * lda];
                    if (LAPACK_HFISNAN(elem)) {
                        has_nan = true;
                        break;
                    }
                }
                if (has_nan) break;
            }
        } else {
            for (int j = 0; j < n; ++j) {
                for (int i = j; i < n; ++i) {
                    if (LAPACK_HFISNAN(a[i + j * lda])) {
                        has_nan = true;
                        break;
                    }
                }
                if (has_nan) break;
            }
        }

        if (has_nan) {
            return (lapack_float)NAN;  // Retornar NaN inmediatamente
        }

        // Si no hay NaN, calcular la norma
        if (lsame_reimpl(uplo, 'U')) {
            for (int j = 0; j < n; ++j) {
                hflassq(j + 1, &a[j], lda, &scale, &sum);
            }
        } else {
            for (int j = 0; j < n; ++j) {
                hflassq(n - j, &a[j * lda + j], lda, &scale, &sum);
            }
        }

        value = scale * custom_sqrtf_half_precision(sum);

        if (!has_nan) {
            // Aplicar saturación solo si no hay NaN
            #ifndef USE_BF16
            if (value > (lapack_float)FP16_MAX) {
                value = (lapack_float)FP16_MAX;
            }
            #else
            if (value > (lapack_float)BF16_MAX) {
                value = (lapack_float)BF16_MAX;
            }
            #endif
        }

    }

    return value;
}

