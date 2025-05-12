
#include "../include/lapacke_utils_reimpl.h"

void hforg2l(lapack_int m, lapack_int n, lapack_int k, _Float16 *a, lapack_int lda,
            _Float16 *tau, _Float16 *work, lapack_int *info) {
    lapack_int i, j, l;

    *info = 0;
    if (m < 0) {
        *info = -1;
    } else if (n < m) {
        *info = -2;
    } else if (k < 0 || k > m) {
        *info = -3;
    } else if (lda < (m > 0 ? m : 1)) {
        *info = -5;
    }
    if (*info != 0) {
        LAPACKE_xerbla("hforg2l", -(*info));
        return;
    }

    if (m == 0) {
        return;
    }

    // Inicializar filas k:m-1 (equivale a k+1:m en Fortran)
    if (k < m) {
        for (j = 0; j < n; j++) {
            for (l = k; l < m; l++) {
                a[j * lda + l] = 0.0F16;
            }
            if (j >= k && j < m) {
                a[j * lda + j] = 1.0F16;
            }
        }
    }

    // Aplicar reflectores inversos (i desde k-1 hasta 0)
    for (i = k - 1; i >= 0; i--) {
        if (i < n - 1) {
            if (i < m - 1) {
                char side = 'R';
                int num_rows = m - i - 1;
                int num_cols = n - i;
                _Float16 *v = &a[i * lda + i];
                int incv = 1;
                _Float16 tau_i = tau[i];
                _Float16 *c_matrix = &a[i * lda + (i + 1)];

                hflarf1f(side, num_rows, num_cols, v, incv, tau_i,
                         c_matrix, lda, work);
            }

            // Escalar A(i, i+1:n) por -tau[i]
            int scal_len = n - i - 1;
            _Float16 alpha = -tau[i];
            int incx = lda;
            hfscal(scal_len, alpha, &a[(i + 1) * lda + i], incx);
        }

        // Actualizar diagonal y ceros a la izquierda
        a[i * lda + i] = 1.0F16 - tau[i];
        for (l = 0; l < i; l++) {
            a[l * lda + i] = 0.0F16;
        }
    }
}