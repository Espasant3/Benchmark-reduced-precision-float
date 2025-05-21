
#include "../include/lapacke_utils_reimpl.h"
/*
void hfaxpy(int n, _Float16 alpha, _Float16 *x, int incx, _Float16 *y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] += alpha * x[i * incx];
    }
}
*/
void hfsytd2(char uplo, lapack_int n, _Float16 *a, int lda, _Float16 *d, _Float16 *e, _Float16 *tau, lapack_int *info) {
    
    // Variables locales
    lapack_logical upper = lsame_reimpl(uplo, 'U');
    int i;
    _Float16 alpha, taui;
    
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

            
            if (taui != 0.0F16) {
                a[i + (i + 1) * lda] = 1.0F16;
                
                // Calcular x = tau * A * v
                hfsymv(uplo, i + 1, taui, a, lda, &a[(i + 1) * lda], 1, 0.0F16, tau, 1);
                
                // Calcular alpha = -1/2 * tau * v^T * x
                alpha = -0.5F16 * taui * hfdot(i + 1, tau, 1, &a[(i + 1) * lda], 1);
                
                // Actualizar x = x - alpha * v
                hfaxpy(i + 1, alpha, &a[(i + 1) * lda], 1, tau, 1);
                
                // Aplicar transformación como rank-2 update
                hfsyr2(uplo, i + 1, -1.0F16, &a[(i + 1) * lda], 1, tau, 1, a, lda);
                
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
            
            if (taui != 0.0F16) {
                a[(i + 1) + i * lda] = 1.0F16;
                
                // Calcular x = tau * A * v
                hfsymv(uplo, n - i - 1, taui, &a[(i + 1) + (i + 1) * lda], lda, 
                       &a[(i + 1) + i * lda], 1, 0.0F16, &tau[i], 1);
                
                // Calcular alpha = -1/2 * tau * v^T * x
                alpha = -0.5F16 * taui * hfdot(n - i - 1, &tau[i], 1, &a[(i + 1) + i * lda], 1);
                
                // Actualizar x = x - alpha * v
                hfaxpy(n - i - 1, alpha, &a[(i + 1) + i * lda], 1, &tau[i], 1);
                
                // Aplicar transformación como rank-2 update
                hfsyr2(uplo, n - i - 1, -1.0F16, &a[(i + 1) + i * lda], 1, &tau[i], 1, &a[(i + 1) + (i + 1) * lda], lda);
                
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