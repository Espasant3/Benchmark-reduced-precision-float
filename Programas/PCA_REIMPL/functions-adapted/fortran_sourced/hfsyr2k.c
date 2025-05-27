
#include "lapacke_utils_reimpl.h"

void hfsyr2k(char uplo, char trans, int n, int k, lapack_float alpha,
            lapack_float *A, int lda, lapack_float *B, int ldb, lapack_float beta,
            lapack_float *C, int ldc) {
    const lapack_float ZERO = (lapack_float)0.0;
    const lapack_float ONE = (lapack_float)1.0;
    int upper = lsame_reimpl(uplo, 'U');
    int nrowa;
    
    if (lsame_reimpl(trans, 'N')) {
        nrowa = n;
    } else {
        nrowa = k;
    }
    
    int info = 0;
    if (!upper && !lsame_reimpl(uplo, 'L')) {
        info = 1;
    } else if (!lsame_reimpl(trans, 'N') && !lsame_reimpl(trans, 'T') && !lsame_reimpl(trans, 'C')) {
        info = 2;
    } else if (n < 0) {
        info = 3;
    } else if (k < 0) {
        info = 4;
    } else if (lda < MAX(nrowa, 1)) {
        info = 7;
    } else if (ldb < MAX(nrowa, 1)) {
        info = 9;
    } else if (ldc < MAX(1, n)) {
        info = 12;
    }
    
    if (info != 0) {
        c_xerbla("HFSYR2K", info);
        return;
    }
    if (n == 0 || ((alpha == ZERO || k == 0) && beta == ONE)) {
        return;
    }
    
    // Caso alpha = 0: C = beta*C
    if (alpha == ZERO) {
        if (upper) {
            if (beta == ZERO) {
                for (int j = 0; j < n; ++j) {  // Índices base 0
                    for (int i = 0; i <= j; ++i) {
                        C[j*ldc + i] = ZERO; //Column-major
                    }
                }
            } else {
                for (int j = 0; j < n; ++j) {
                    for (int i = 0; i <= j; ++i) {
                        C[j*ldc + i] *= beta; //Column-major
                    }
                }
            }
        } else {
            if (beta == ZERO) {
                for (int j = 0; j < n; ++j) {
                    for (int i = j; i < n; ++i) {
                        C[j*ldc + i] = ZERO; //Column-major
                    }
                }
            } else {
                for (int j = 0; j < n; ++j) {
                    for (int i = j; i < n; ++i) {
                        C[j*ldc + i] *= beta; //Column-major
                    }
                }
            }
        }
        return;
    }
    
    // Operaciones principales
    if (lsame_reimpl(trans, 'N')) {
        if (upper) {
            for (int j = 0; j < n; ++j) {  // Índices base 0
                // Escalar C por beta
                if (beta == ZERO) {
                    for (int i = 0; i <= j; ++i) {
                        C[j*ldc + i] = ZERO; // Column-major
                    }
                } else if (beta != ONE) {
                    for (int i = 0; i <= j; ++i) {
                        C[j*ldc + i] *= beta; // Column-major
                    }
                }
                // C += alpha*A*B^T + alpha*B*A^T
                for (int l = 0; l < k; ++l) {
                    lapack_float a_jl = A[l*lda + j]; // Column-major
                    lapack_float b_jl = B[l*ldb + j]; // Column-major
                    if (a_jl != ZERO || b_jl != ZERO) {
                        lapack_float temp1 = alpha * b_jl;
                        lapack_float temp2 = alpha * a_jl;
                        for (int i = 0; i <= j; ++i) {
                            C[j*ldc + i] += A[l*lda + i] * temp1 + B[l*ldb + i] * temp2; // Column-major
                        }
                    }
                }
            }
        } else {
            // Lógica similar para Lower
            for (int j = 0; j < n; ++j) {
                if (beta == ZERO) {
                    for (int i = j; i < n; ++i) {
                        C[j*ldc + i] = ZERO; // Column-major
                    }
                } else if (beta != ONE) {
                    for (int i = j; i < n; ++i) {
                        C[j*ldc + i] *= beta; // Column-major
                    }
                }
                for (int l = 0; l < k; ++l) {
                    lapack_float a_jl = A[l*lda + j]; // Column-major
                    lapack_float b_jl = B[l*ldb + j]; // Column-major
                    if (a_jl != ZERO || b_jl != ZERO) {
                        lapack_float temp1 = alpha * b_jl;
                        lapack_float temp2 = alpha * a_jl;
                        for (int i = j; i < n; ++i) {
                            C[j*ldc + i] += A[l*lda + i] * temp1 + B[l*ldb + i] * temp2; // Column-major
                        }
                    }
                }
            }
        }
    } else {  // Transposed case (TRANS = 'T' o 'C')
        if (upper) {
            for (int j = 0; j < n; ++j) {
                for (int i = 0; i <= j; ++i) {
                    lapack_float temp1 = ZERO, temp2 = ZERO;
                    for (int l = 0; l < k; ++l) {
                        temp1 += A[i*lda + l] * B[j*ldb + l]; // Column-major
                        temp2 += B[i*ldb + l] * A[j*lda + l]; // Column-major
                    }
                    // Conversión final a lapack_float con escalado
                    lapack_float acc = alpha * (temp1 + temp2);
                    if (beta == ZERO) {
                        C[j*ldc + i] = acc; // Column-major
                    } else {
                        C[j*ldc + i] = beta * C[j*ldc + i] + acc; // Column-major
                    }
                }
            }
        } else {
            for (int j = 0; j < n; ++j) {
                for (int i = j; i < n; ++i) {
                    lapack_float temp1 = ZERO, temp2 = ZERO;
                    for (int l = 0; l < k; ++l) {
                        temp1 += A[i*lda + l] * B[j*ldb + l]; // Column-major
                        temp2 += B[i*ldb + l] * A[j*lda + l]; // Column-major
                    }
                    
                    lapack_float acc = alpha * (temp1 + temp2);
                    if (beta == ZERO) {
                        C[j*ldc + i] = acc; // Column-major
                    } else {
                        C[j*ldc + i] = beta * C[j*ldc + i] + acc; // Column-major
                    }
                }
            }
        }
    }
}