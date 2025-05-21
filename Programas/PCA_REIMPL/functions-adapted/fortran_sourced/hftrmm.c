
#include "../include/lapacke_utils_reimpl.h"

void hftrmm(const char side, const char uplo, const char transa, const char diag,
           const int m, const int n, const _Float16 alpha, const _Float16 *a, const int lda,
           _Float16 *b, const int ldb) {

    lapack_logical lside, nounit, upper;
    int info, nrowa, i, j, k;
    _Float16 temp;

    const _Float16 one = 1.0F16;
    const _Float16 zero = 0.0F16;

    // Determinar parámetros y validar entradas
    lside = lsame_reimpl(side, 'L');
    nrowa = lside ? m : n;
    nounit = lsame_reimpl(diag, 'N');
    upper = lsame_reimpl(uplo, 'U');

    info = 0;
    if (!lside && !lsame_reimpl(side, 'R')) {
        info = 1;
    } else if (!upper && !lsame_reimpl(uplo, 'L')) {
        info = 2;
    } else if (!lsame_reimpl(transa, 'N') && 
               !lsame_reimpl(transa, 'T') && 
               !lsame_reimpl(transa, 'C')) {
        info = 3;
    } else if (!lsame_reimpl(diag, 'U') && 
               !lsame_reimpl(diag, 'N')) {
        info = 4;
    } else if (m < 0) {
        info = 5;
    } else if (n < 0) {
        info = 6;
    } else if (lda < MAX(1, nrowa)) {
        info = 9;
    } else if (ldb < MAX(1, m)) {
        info = 11;
    }

    if (info != 0) {
        c_xerbla("hftrmm", info);
        return;
    }

    // Salida rápida si no hay trabajo
    if (m == 0 || n == 0) return;

    // Caso alpha = 0
    if (alpha == zero) {
        for (j = 0; j < n; ++j) {
            for (i = 0; i < m; ++i) {
                b[i + j * ldb] = zero;
            }
        }
        return;
    }

    // Operaciones principales
    if (lside) {
        if (lsame_reimpl(transa, 'N')) {
            // B = alpha*A*B
            if (upper) {
                for (j = 0; j < n; ++j) {
                    for (k = 0; k < m; ++k) {
                        if (b[k + j * ldb] != zero) {
                            temp = alpha * b[k + j * ldb];
                            for (i = 0; i < k; ++i) {
                                b[i + j * ldb] += temp * a[i + k * lda];
                            }
                            if (nounit) {
                                temp *= a[k + k * lda];
                            }
                            b[k + j * ldb] = temp;
                        }
                    }
                }
            } else {
                for (j = 0; j < n; ++j) {
                    for (k = m - 1; k >= 0; --k) {
                        if (b[k + j * ldb] != zero) {
                            temp = alpha * b[k + j * ldb];
                            _Float16 temp_update = temp;
                            if (nounit) {
                                temp *= a[k + k * lda];
                            }
                            b[k + j * ldb] = temp;
                            for (i = k + 1; i < m; ++i) {
                                b[i + j * ldb] += temp_update * a[i + k * lda];
                            }
                        }
                    }
                }
            }
        } else {
            // B = alpha*A^T*B
            if (upper) {
                for (j = 0; j < n; ++j) {
                    for (i = m - 1; i >= 0; --i) {
                        temp = b[i + j * ldb];
                        if (nounit) {
                            temp *= a[i + i * lda];
                        }
                        for (k = 0; k < i; ++k) {
                            temp += a[k + i * lda] * b[k + j * ldb];
                        }
                        b[i + j * ldb] = alpha * temp;
                    }
                }
            } else {
                for (j = 0; j < n; ++j) {
                    for (i = 0; i < m; ++i) {
                        temp = b[i + j * ldb];
                        if (nounit) {
                            temp *= a[i + i * lda];
                        }
                        for (k = i + 1; k < m; ++k) {
                            temp += a[k + i * lda] * b[k + j * ldb];
                        }
                        b[i + j * ldb] = alpha * temp;
                    }
                }
            }
        }
    } else {
        if (lsame_reimpl(transa, 'N')) {
            // B = alpha*B*A
            if (upper) {
                for (j = n - 1; j >= 0; --j) {
                    temp = alpha;
                    if (nounit) {
                        temp *= a[j + j * lda];
                    }
                    for (i = 0; i < m; ++i) {
                        b[i + j * ldb] *= temp;
                    }
                    for (k = 0; k < j; ++k) {
                        if (a[k + j * lda] != zero) {
                            temp = alpha * a[k + j * lda];
                            for (i = 0; i < m; ++i) {
                                b[i + j * ldb] += temp * b[i + k * ldb];
                            }
                        }
                    }
                }
            } else {
                for (j = 0; j < n; ++j) {
                    temp = alpha;
                    if (nounit) {
                        temp *= a[j + j * lda];
                    }
                    for (i = 0; i < m; ++i) {
                        b[i + j * ldb] *= temp;
                    }
                    for (k = j + 1; k < n; ++k) {
                        if (a[k + j * lda] != zero) {
                            temp = alpha * a[k + j * lda];
                            for (i = 0; i < m; ++i) {
                                b[i + j * ldb] += temp * b[i + k * ldb];
                            }
                        }
                    }
                }
            }
        } else {
            // B = alpha*B*A^T
            if (upper) {
                for (k = 0; k < n; ++k) {
                    for (j = 0; j < k; ++j) {
                        if (a[j + k * lda] != zero) {
                            temp = alpha * a[j + k * lda];
                            for (i = 0; i < m; ++i) {
                                b[i + j * ldb] += temp * b[i + k * ldb];
                            }
                        }
                    }
                    temp = alpha;
                    if (nounit) {
                        temp *= a[k + k * lda];
                    }
                    if (temp != one) {
                        for (i = 0; i < m; ++i) {
                            b[i + k * ldb] *= temp;
                        }
                    }
                }
            } else {
                for (k = n - 1; k >= 0; --k) {
                    for (j = k + 1; j < n; ++j) {
                        if (a[j + k * lda] != zero) {
                            temp = alpha * a[j + k * lda];
                            for (i = 0; i < m; ++i) {
                                b[i + j * ldb] += temp * b[i + k * ldb];
                            }
                        }
                    }
                    temp = alpha;
                    if (nounit) {
                        temp *= a[k + k * lda];
                    }
                    if (temp != one) {
                        for (i = 0; i < m; ++i) {
                            b[i + k * ldb] *= temp;
                        }
                    }
                }
            }
        }
    }
}