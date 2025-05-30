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

void hfsytd2(char uplo, lapack_int n, lapack_float *a, int lda, lapack_float *d, lapack_float *e, lapack_float *tau, lapack_int *info) {
    
    // Constantes
    const lapack_float ZERO = (lapack_float)0.0;
    const lapack_float ONE = (lapack_float)1.0;
    const lapack_float HALF = (lapack_float)0.5;

    // Variables locales
    lapack_logical upper = lsame_reimpl(uplo, 'U');
    int i;
    lapack_float alpha, taui;
    
    // Verificar parámetros de entrada
    *info = 0;
    if (!upper && !lsame_reimpl(uplo, 'L')) {
        *info = -1;
    } else if (n < 0) {
        *info = -2;
    } else if (lda < MAX(1, n)) {
        *info = -4;
    }
    if (*info != 0) {
        LAPACKE_xerbla("HFSYTD2", -(*info));
        return;
    }
    // Retorno rápido si es necesario
    if (n <= 0) return;
    
    if (upper) {
        // Reducir parte superior
        for (i = n - 2; i >= 0; i--) {
            // Generar reflector de Householder
            hflarfg(i + 1, &a[i + (i + 1) * lda], &a[(i + 1) * lda], 1, &taui);

            
            if (taui != ZERO) {
                a[i + (i + 1) * lda] = ONE;
                
                // Calcular x = tau * A * v
                hfsymv(uplo, i + 1, taui, a, lda, &a[(i + 1) * lda], 1, ZERO, tau, 1);
                
                // Calcular alpha = -1/2 * tau * v^T * x
                alpha = -HALF * taui * hfdot(i + 1, tau, 1, &a[(i + 1) * lda], 1);
                
                // Actualizar x = x - alpha * v
                hfaxpy(i + 1, alpha, &a[(i + 1) * lda], 1, tau, 1);
                
                // Aplicar transformación como rank-2 update
                hfsyr2(uplo, i + 1, -ONE, &a[(i + 1) * lda], 1, tau, 1, a, lda);
                
                // Restaurar valor original
                a[i + (i + 1) * lda] = e[i];
            }
            d[i + 1] = a[(i + 1) + (i + 1) * lda];
            tau[i] = taui;
        }
        d[0] = a[0];
    } else {
        // Reducir parte inferior
        for (i = 0; i < n - 1; i++) {
            
            // Generar reflector de Householder
            //LAPACKE_hflarfg(dim, &a[(i + 1) + i * lda], &a[(i + 2) + i * lda], ONE, &taui);
            hflarfg(n - i - 1, &a[(i + 1) + i * lda], &a[MIN(i + 2, n) + i * lda], 1, &taui);

            e[i] = a[(i + 1) + i * lda];
            
            if (taui != ZERO) {
                a[(i + 1) + i * lda] = ONE;
                
                // Calcular x = tau * A * v
                hfsymv(uplo, n - i - 1, taui, &a[(i + 1) + (i + 1) * lda], lda, 
                       &a[(i + 1) + i * lda], 1, ZERO, &tau[i], 1);
                
                // Calcular alpha = -1/2 * tau * v^T * x
                alpha = -HALF * taui * hfdot(n - i - 1, &tau[i], 1, &a[(i + 1) + i * lda], 1);
                
                // Actualizar x = x - alpha * v
                hfaxpy(n - i - 1, alpha, &a[(i + 1) + i * lda], 1, &tau[i], 1);
                
                // Aplicar transformación como rank-2 update
                hfsyr2(uplo, n - i - 1, -ONE, &a[(i + 1) + i * lda], 1, &tau[i], 1, &a[(i + 1) + (i + 1) * lda], lda);
                
                // Restaurar valor original
                a[(i + 1) + i * lda] = e[i];
            }
            d[i] = a[i + i * lda];
            tau[i] = taui;
        }
        d[n - 1] = a[(n - 1) + (n - 1) * lda];
    }
    return;    
}