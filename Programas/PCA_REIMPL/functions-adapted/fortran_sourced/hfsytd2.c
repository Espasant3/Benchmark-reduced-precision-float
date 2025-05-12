
#include "../include/lapacke_utils_reimpl.h"
/*
void hfaxpy(int n, _Float16 alpha, _Float16 *x, int incx, _Float16 *y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] += alpha * x[i * incx];
    }
}
*/
void hfsytd2(char uplo, lapack_int n, _Float16 *a, int lda, _Float16 *d, _Float16 *e, _Float16 *tau, lapack_int *info) {
    printf("LAPACKE_hfsytd2: n = %d, lda = %d\n", n, lda);
    // Constantes
    const _Float16 ZERO = 0.0F16, HALF = 0.5F16, MINUS_ONE = -1.0F16;
    const int ONE = 1;
    
    // Variables locales
    bool upper = lsame_reimpl(uplo, 'U');
    int i;
    _Float16 alpha, taui;
    
    // Verificar parámetros de entrada
    *info = 0;
    if (!upper && (uplo != 'L' && uplo != 'l')) {
        *info = -1;
    } else if (n < 0) {
        *info = -2;
    } else if (lda < (n > 0 ? n : 1)) {
        *info = -4;
    }
    if (*info != 0) {
        LAPACKE_xerbla("HFSYTD2", *info);
        return;
    }
    printf("LAPACKE_hfsytd2: n = %d, lda = %d\n", n, lda);
    // Retorno rápido si es necesario
    if (n <= 0) return;
    
    if (upper) {
        // Reducir parte superior
        for (i = n - 2; i >= 0; i--) {
            lapack_int dim = i + 1;  // Tamaño del reflector
            lapack_int col = i + 1;  // Columna actual (0-based)
            
            // Generar reflector de Householder
            //LAPACKE_hflarfg(dim, &a[i + col * lda], &a[col * lda], ONE, &taui);
            hflarfg(dim, &a[i + col * lda], &a[col * lda], ONE, &taui);

            e[i] = a[i + col * lda];
            
            if (taui != ZERO) {
                a[i + col * lda] = 1.0F;
                
                // Calcular x = tau * A * v
                hfsymv(uplo, dim, taui, a, lda, &a[col * lda], ONE, ZERO, tau, ONE);
                
                // Calcular alpha = -1/2 * tau * v^T * x
                alpha = -HALF * taui * hfdot(dim, tau, ONE, &a[col * lda], ONE);
                
                // Actualizar x = x - alpha * v
                hfaxpy(dim, alpha, &a[col * lda], ONE, tau, ONE);
                
                // Aplicar transformación como rank-2 update
                hfsyr2(uplo, dim, MINUS_ONE, &a[col * lda], ONE, tau, ONE, a, lda);
                
                // Restaurar valor original
                a[i + col * lda] = e[i];
            }
            d[i + 1] = a[(i + 1) + (i + 1) * lda];
            tau[i] = taui;
        }
        d[0] = a[0];
    } else {
        // Reducir parte inferior
        for (i = 0; i < n - 1; i++) {
            lapack_int dim = n - i - 1;  // Tamaño del reflector
            
            // Generar reflector de Householder
            //LAPACKE_hflarfg(dim, &a[(i + 1) + i * lda], &a[(i + 2) + i * lda], ONE, &taui);
            hflarfg(dim, &a[(i + 1) + i * lda], &a[(i + 2) + i * lda], ONE, &taui);

            e[i] = a[(i + 1) + i * lda];
            
            if (taui != ZERO) {
                a[(i + 1) + i * lda] = 1.0F;
                
                // Calcular x = tau * A * v
                hfsymv(uplo, dim, taui, &a[(i + 1) + (i + 1) * lda], lda, 
                       &a[(i + 1) + i * lda], ONE, ZERO, &tau[i], ONE);
                
                // Calcular alpha = -1/2 * tau * v^T * x
                alpha = -HALF * taui * hfdot(dim, &tau[i], ONE, &a[(i + 1) + i * lda], ONE);
                
                // Actualizar x = x - alpha * v
                hfaxpy(dim, alpha, &a[(i + 1) + i * lda], ONE, &tau[i], ONE);
                
                // Aplicar transformación como rank-2 update
                hfsyr2(uplo, dim, MINUS_ONE, &a[(i + 1) + i * lda], ONE, &tau[i], ONE, &a[(i + 1) + (i + 1) * lda], lda);
                
                // Restaurar valor original
                a[(i + 1) + i * lda] = e[i];
            }
            d[i] = a[i + i * lda];
            tau[i] = taui;
        }
        d[n - 1] = a[(n - 1) + (n - 1) * lda];
    }
    
    *info = 0;
}