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

void hflatrd(char uplo, int n, int nb, lapack_float *a, int lda, lapack_float *e, lapack_float *tau, lapack_float *w, int ldw) {
    
    // Constantes
    const lapack_float ZERO = (lapack_float) 0.0; 
    const lapack_float ONE = (lapack_float) 1.0;
    const lapack_float HALF = (lapack_float) 0.5;
    
    int i, iw;
    lapack_float alpha;

    if (n <= 0 || nb <= 0) return;

    if (lsame_reimpl(uplo, 'U')) {

        for(i = n ; i >= n - nb + 1; i--){
            iw = i - n + nb - 1;
            if(i < n){
                hfgemv('N', i, n - i, -ONE, &a[i * lda], lda, 
                       &w[(i - 1) + (iw + 1) * ldw], ldw, 
                       ONE, &a[(i - 1) * lda], 1);
                hfgemv('N', i, n - i, -ONE, &w[(iw + 1) * ldw], ldw,
                       &a[(i - 1) + i * lda], lda, 
                       ONE, &a[(i - 1) * lda], 1);
            }

            if(i > 1){
                hflarfg(i - 1, &a[(i - 2) + (i - 1) * lda], &a[(i - 1) * lda], 1, &tau[i - 2]);
                e[i - 2] = a[(i - 2) + (i - 1) * lda];
                a[(i - 2) + (i - 1) * lda] = ONE;

                hfsymv('U', i - 1, ONE, a, lda, &a[(i - 1) * lda], 1, ZERO, &w[iw * ldw], 1);

                if(i < n){
                    hfgemv('T', i - 1, n - i, ONE, &w[(iw + 1) * ldw], ldw, 
                           &a[(i - 1) * lda], 1, ZERO, &w[i + iw * ldw], 1);
                    hfgemv('N', i - 1, n - i, -ONE, &a[i * lda], lda,
                           &w[i + iw * ldw], 1, ONE, &w[iw * ldw], 1);
                    hfgemv('T', i - 1, n - i, ONE, &a[i * lda], lda,
                           &a[(i - 1) * lda], 1, ZERO, &w[i + iw * ldw], 1);
                    hfgemv('N', i - 1, n - i, -ONE, &w[(iw + 1) * ldw], ldw,
                           &w[i + iw * ldw], 1, ONE, &w[iw * ldw], 1);
                }
                hfscal(i - 1, tau[i - 2], &w[iw * ldw], 1);
                alpha = -HALF * tau[i - 2] * hfdot(i - 1, &w[iw * ldw], 1, &a[(i - 1) * lda], 1);
                hfaxpy(i - 1, alpha, &a[(i - 1) * lda], 1, &w[iw * ldw], 1);
            }
        }

    } else {

        for(i = 1; i <= nb ; i++){

            hfgemv('N', n - i + 1, i - 1, -ONE, &a[i - 1], lda, 
                   &w[i - 1], ldw, ONE, &a[(i - 1) + (i - 1) * lda], 1);
            hfgemv('N', n - i + 1, i - 1, -ONE, &w[i - 1], ldw,
                   &a[i - 1], lda, ONE, &a[(i - 1) + (i - 1) * lda], 1);
            if(i < n){
                
                hflarfg(n - i , &a[i + (i - 1) * lda], &a[MIN(i + 1, n - 1) + (i - 1) * lda], 1, &tau[i - 1]);
                e[i - 1] = a[i + (i - 1) * lda];
                a[i + (i - 1) * lda] = ONE;

                hfsymv('L', n - i, ONE, &a[i + i * lda], lda, &a[i + (i - 1) * lda], 1, ZERO, &w[i + (i - 1) * ldw], 1);

                hfgemv('T', n - i, i - 1, ONE, &w[i], ldw, &a[i + (i - 1) * lda], 1, ZERO, &w[(i - 1) * ldw], 1);
                hfgemv('N', n - i, i - 1, -ONE, &a[i], lda, &w[(i - 1) * ldw], 1, ONE, &w[i + (i - 1) * ldw], 1);
                hfgemv('T', n - i, i - 1, ZERO, &a[i], lda, &a[i + (i - 1) * lda], 1, ZERO, &w[(i - 1) * ldw], 1);
                hfgemv('N', n - i, i - 1, -ONE, &w[i], ldw, &w[(i - 1) * ldw], 1, ONE, &w[i + (i - 1) * ldw], 1);

                hfscal(n - i, tau[i - 1], &w[i + (i - 1) * ldw], 1);

                alpha = -HALF * tau[i - 1] * hfdot(n - i, &w[i + (i - 1) * ldw], 1, &a[i + (i - 1) * lda], 1);

                hfaxpy(n - i, alpha, &a[i + (i - 1) * lda], 1, &w[i + (i - 1) * ldw], 1);
            
            }
        }
    }
}