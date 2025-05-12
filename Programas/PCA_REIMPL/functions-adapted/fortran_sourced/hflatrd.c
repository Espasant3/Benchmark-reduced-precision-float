
#include "../include/lapacke_utils_reimpl.h"

#define INC_0 0
#define INC_1 1
/*
void hfaxpy(int n, _Float16 alpha, _Float16 *x, int incx, _Float16 *y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] += alpha * x[i * incx];
    }
}
*/
void hflatrd(char uplo, int n, int nb, _Float16 *a, int lda, _Float16 *e, _Float16 *tau, _Float16 *w, int ldw) {
    const _Float16 ZERO_F16 = 0.0F16, ONE_F16 = 1.0F16, MINUS_ONE_F16 = -1.0F16, HALF_F16 = 0.5F16;
    int i, iw;
    _Float16 alpha;
    char blas_uplo = toupper(uplo); // 'U' o 'L' para BLAS
    char trans_n = 'N', trans_t = 'T'; // Operaciones NoTrans y Trans

    if (n <= 0 || nb <= 0) return;

    if (blas_uplo == 'U') {
        // Reducir las últimas NB columnas de la parte superior
        for (i = n - 1; i >= n - nb; --i) {
            iw = i - (n - nb); // Índice en W (ajuste 0-based)
            if (i < n - 1) {
                // Actualizar A(0:i, i) = A(0:i, i) - A(0:i, i+1:n-1) * W(i, iw+1:nb-1)
                int m = i + 1; // Filas: I en Fortran (1-based)
                int n_cols = n - i - 1; // Columnas: N-I-1 (0-based)
                hfgemv(trans_n, m, n_cols, MINUS_ONE_F16, 
                       &a[(i + 1) * lda], lda, 
                       &w[i + (iw + 1) * ldw], ldw, 
                       ONE_F16, &a[i * lda], INC_1);
                hfgemv(trans_n, m, n_cols, MINUS_ONE_F16, 
                       &w[(iw + 1) * ldw], ldw, 
                       &a[i + (i + 1) * lda], lda, 
                       ONE_F16, &a[i * lda], INC_1);
            }
            if (i > 0) {
                // Generar reflector H(i) para annihilar A(0:i-2, i)
                _Float16 *x = &a[i * lda]; // x = A(0:i-1, i)
                int len = i;
                hflarfg(len, &x[i - 1], x, INC_1, &tau[i - 1]);
                e[i - 1] = x[i - 1]; // Guardar en E[i-1] (0-based)
                x[i - 1] = 1.0F16;

                // Calcular W(0:i-1, iw) = A(0:i-1, 0:i-1) * x
                int symv_n = i;
                hfsymv(blas_uplo, symv_n, ONE_F16, a, lda, x, INC_1, ZERO_F16, &w[iw * ldw], INC_1);

                if (i < n - 1) {
                    int n_cols = n - i - 1;
                    _Float16 *work = (_Float16*)malloc(n_cols * sizeof(_Float16));
                    // W(0:i-1, iw) -= A(0:i-1, i+1:n-1) * W(i, iw+1:nb-1)^T * x
                    hfgemv(trans_t, i, n_cols, ONE_F16, 
                           &w[(iw + 1) * ldw], ldw, 
                           x, INC_1, ZERO_F16, work, INC_1);
                    hfgemv(trans_n, i, n_cols, MINUS_ONE_F16, 
                           &a[(i + 1) * lda], lda, 
                           work, INC_1, ONE_F16, &w[iw * ldw], INC_1);
                    // W(0:i-1, iw) -= W(0:i-1, iw+1:nb-1) * A(i, i+1:n-1)^T * x
                    hfgemv(trans_t, i, n_cols, ONE_F16, 
                           &a[(i + 1) * lda], lda, 
                           x, INC_1, ZERO_F16, work, INC_1);
                    hfgemv(trans_n, i, n_cols, MINUS_ONE_F16, 
                           &w[(iw + 1) * ldw], ldw, 
                           work, INC_1, ONE_F16, &w[iw * ldw], INC_1);
                    free(work);
                }
                // Escalar W(0:i-1, iw) por tau[i-1] y ajustar
                hfscal(i, tau[i - 1], &w[iw * ldw], INC_1);
                alpha = -HALF_F16 * tau[i - 1] * hfdot(i, &w[iw * ldw], INC_1, x, INC_1);
                hfaxpy(i, alpha, x, INC_1, &w[iw * ldw], INC_1);
            }
        }
    } else {
        // Reducir las primeras NB columnas de la parte inferior
        for (i = 0; i < nb; ++i) {
            iw = i;
            if (i < n - 1) {
                // Actualizar A(i:n-1, i) -= A(i:n-1, 0:i-1) * W(i:n-1, 0:i-1)
                int m = n - i;
                hfgemv(trans_n, m, i, MINUS_ONE_F16, 
                       &a[i], lda, &w[i], ldw, 
                       ONE_F16, &a[i + i * lda], INC_1);
                hfgemv(trans_n, m, i, MINUS_ONE_F16, 
                       &w[i], ldw, &a[i], lda, 
                       ONE_F16, &a[i + i * lda], INC_1);
            }
            if (i < n - 1) {
                // Generar reflector H(i) para annihilar A(i+2:n-1, i)
                _Float16 *x = &a[(i + 1) + i * lda]; // x = A(i+1:n-1, i)
                int len = n - i - 1;
                hflarfg(len, &x[0], &x[1], INC_1, &tau[i]);
                e[i] = x[0]; // Guardar en E[i] (0-based)
                x[0] = 1.0F16;

                // Calcular W(i+1:n-1, iw) = A(i+1:n-1, i+1:n-1) * x
                int symv_n = n - i - 1;
                hfsymv(blas_uplo, symv_n, ONE_F16, &a[(i + 1) + (i + 1) * lda], lda, x, INC_1, ZERO_F16, &w[i + 1 + iw * ldw], INC_1);

                _Float16 *work = (_Float16*)malloc(i * sizeof(_Float16));
                // W(i+1:n-1, iw) -= A(i+1:n-1, 0:i-1) * W(i+1:n-1, 0:i-1)^T * x
                hfgemv(trans_t, symv_n, i, ONE_F16, 
                       &w[i + 1], ldw, 
                       x, INC_1, ZERO_F16, work, INC_1);
                hfgemv(trans_n, symv_n, i, MINUS_ONE_F16, 
                       &a[i + 1], lda, 
                       work, INC_1, ONE_F16, &w[i + 1 + iw * ldw], INC_1);
                // W(i+1:n-1, iw) -= W(i+1:n-1, 0:i-1) * A(i+1:n-1, 0:i-1)^T * x
                hfgemv(trans_t, symv_n, i, ONE_F16, 
                       &a[i + 1], lda, 
                       x, INC_1, ZERO_F16, work, INC_1);
                hfgemv(trans_n, symv_n, i, MINUS_ONE_F16, 
                       &w[i + 1], ldw, 
                       work, INC_1, ONE_F16, &w[i + 1 + iw * ldw], INC_1);
                free(work);

                // Escalar W(i+1:n-1, iw) por tau[i] y ajustar
                hfscal(symv_n, tau[i], &w[i + 1 + iw * ldw], INC_1);
                alpha = -HALF_F16 * tau[i] * hfdot(symv_n, &w[i + 1 + iw * ldw], INC_1, x, INC_1);
                hfaxpy(symv_n, alpha, x, INC_1, &w[i + 1 + iw * ldw], INC_1);
            }
        }
    }
}