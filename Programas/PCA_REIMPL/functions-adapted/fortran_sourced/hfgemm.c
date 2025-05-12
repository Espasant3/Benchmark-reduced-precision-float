
#include "../include/lapacke_utils_reimpl.h"

void hfgemm(char transa, char transb, int m, int n, int k,
           _Float16 alpha, const _Float16 *a, int lda,
           const _Float16 *b, int ldb, _Float16 beta,
           _Float16 *c, int ldc) {

    lapack_logical nota, notb;
    int nrowa, nrowb;
    int info = 0;
    int i, j, l;
    _Float16 temp;

    const _Float16 one = 1.0F16;
    const _Float16 zero = 0.0F16;

    nota = lsame_reimpl(transa, 'N');
    notb = lsame_reimpl(transb, 'N');

    nrowa = nota ? m : k;
    nrowb = notb ? k : n;

    // Validación de parámetros (igual que antes)
    if ((!nota) && !lsame_reimpl(transa, 'T') && !lsame_reimpl(transa, 'C')) {
        info = 1;
    } else if ((!notb) && !lsame_reimpl(transb, 'T') && !lsame_reimpl(transb, 'C')) {
        info = 2;
    } else if (m < 0) {
        info = 3;
    } else if (n < 0) {
        info = 4;
    } else if (k < 0) {
        info = 5;
    } else if (lda < MAX(1, nrowa)) {
        info = 8;
    } else if (ldb < MAX(1, nrowb)) {
        info = 10;
    } else if (ldc < MAX(1, m)) {
        info = 13;
    }

    if (info != 0) {
        c_xerbla("HFGEMM ", info);
        return;
    }

    // Retorno rápido (igual que antes)
    if (m == 0 || n == 0 || ((alpha == zero || k == 0) && beta == one)) {
        return;
    }

    // Manejo de alpha == zero (igual que antes)
    if (alpha == zero) {
        if (beta == zero) {
            for (j = 0; j < n; j++) {
                for (i = 0; i < m; i++) {
                    c[i + j * ldc] = zero;
                }
            }
        } else {
            for (j = 0; j < n; j++) {
                for (i = 0; i < m; i++) {
                    c[i + j * ldc] *= beta;
                }
            }
        }
        return;
    }

    // Casos de multiplicación (lógica idéntica)
    if (notb) {
        if (nota) {
            // C = alpha*A*B + beta*C
            for (j = 0; j < n; j++) {
                if (beta == zero) {
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] = zero;
                    }
                } else if (beta != one) {
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] *= beta;
                    }
                }
                for (l = 0; l < k; l++) {
                    temp = alpha * b[l + j * ldb];
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] += temp * a[i + l * lda];
                    }
                }
            }
        } else {
            // C = alpha*A^T*B + beta*C
            for (j = 0; j < n; j++) {
                for (i = 0; i < m; i++) {
                    temp = zero;
                    for (l = 0; l < k; l++) {
                        temp += a[l + i * lda] * b[l + j * ldb];
                    }
                    if (beta == zero) {
                        c[i + j * ldc] = alpha * temp;
                    } else {
                        c[i + j * ldc] = alpha * temp + beta * c[i + j * ldc];
                    }
                }
            }
        }
    } else {
        if (nota) {
            // C = alpha*A*B^T + beta*C
            for (j = 0; j < n; j++) {
                if (beta == zero) {
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] = zero;
                    }
                } else if (beta != one) {
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] *= beta;
                    }
                }
                for (l = 0; l < k; l++) {
                    temp = alpha * b[j + l * ldb];
                    for (i = 0; i < m; i++) {
                        c[i + j * ldc] += temp * a[i + l * lda];
                    }
                }
            }
        } else {
            // C = alpha*A^T*B^T + beta*C
            for (j = 0; j < n; j++) {
                for (i = 0; i < m; i++) {
                    temp = zero;
                    for (l = 0; l < k; l++) {
                        temp += a[l + i * lda] * b[j + l * ldb];
                    }
                    if (beta == zero) {
                        c[i + j * ldc] = alpha * temp;
                    } else {
                        c[i + j * ldc] = alpha * temp + beta * c[i + j * ldc];
                    }
                }
            }
        }
    }
}