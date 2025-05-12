#include "../include/lapacke_utils_reimpl.h"

void hfsyr2k(char uplo, char trans, int n, int k, _Float16 alpha,
            _Float16 *A, int lda, _Float16 *B, int ldb, _Float16 beta,
            _Float16 *C, int ldc) {
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
    if (n == 0 || ((alpha == 0.0F16 || k == 0) && beta == 1.0F16)) {
        return;
    }
    
    // Caso alpha = 0: C = beta*C
    if (alpha == 0.0F16) {
        if (upper) {
            if (beta == 0.0F16) {
                for (int j = 0; j < n; ++j) {  // Índices base 0
                    for (int i = 0; i <= j; ++i) {
                        //C[j*ldc + i] = 0.0F16; //Column-major
                        C[i*ldc + j] = 0.0F16;   //Row-major
                    }
                }
            } else {
                for (int j = 0; j < n; ++j) {
                    for (int i = 0; i <= j; ++i) {
                        //C[j*ldc + i] *= beta; //Column-major
                        C[i*ldc + j] *= beta;   //Row-major
                    }
                }
            }
        } else {
            if (beta == 0.0F16) {
                for (int j = 0; j < n; ++j) {
                    for (int i = j; i < n; ++i) {
                        //C[j*ldc + i] = 0.0F16; //Column-major
                        C[i*ldc + j] = 0.0F16;   //Row-major
                    }
                }
            } else {
                for (int j = 0; j < n; ++j) {
                    for (int i = j; i < n; ++i) {
                        //C[j*ldc + i] *= beta; //Column-major
                        C[i*ldc + j] *= beta;   //Row-major
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
                if (beta == 0.0F16) {
                    for (int i = 0; i <= j; ++i) {
                        //C[j*ldc + i] = 0.0F16; // Column-major
                        C[i*ldc + j] = 0.0F16;   // Row-major
                    }
                } else if (beta != 1.0F16) {
                    for (int i = 0; i <= j; ++i) {
                        //C[j*ldc + i] *= beta; // Column-major
                        C[i*ldc + j] *= beta;   // Row-major
                    }
                }
                // C += alpha*A*B^T + alpha*B*A^T
                for (int l = 0; l < k; ++l) {
                    //_Float16 a_jl = A[l*lda + j]; // Column-major
                    _Float16 a_jl = A[j*lda + l];   // Row-major
                    //_Float16 b_jl = B[l*ldb + j]; // Column-major
                    _Float16 b_jl = B[j*ldb + l];   // Row-major
                    if (a_jl != 0.0F16 || b_jl != 0.0F16) {
                        _Float16 temp1 = alpha * b_jl;
                        _Float16 temp2 = alpha * a_jl;
                        for (int i = 0; i <= j; ++i) {
                            //C[j*ldc + i] += A[l*lda + i] * temp1 + B[l*ldb + i] * temp2; // Column-major
                            C[i*ldc + j] += A[i*lda + l] * temp1 + B[i*ldb + l] * temp2;   // Row-major
                        }
                    }
                }
            }
        } else {
            // Lógica similar para Lower
            for (int j = 0; j < n; ++j) {
                if (beta == 0.0F16) {
                    for (int i = j; i < n; ++i) {
                        //C[j*ldc + i] = 0.0F16; // Column-major
                        C[i*ldc + j] = 0.0F16;   // Row-major
                    }
                } else if (beta != 1.0F16) {
                    for (int i = j; i < n; ++i) {
                        //C[j*ldc + i] *= beta; // Column-major
                        C[i*ldc + j] *= beta;   // Row-major
                    }
                }
                for (int l = 0; l < k; ++l) {
                    //_Float16 a_jl = A[l*lda + j]; // Column-major
                    _Float16 a_jl = A[j*lda + l];   // Row-major
                    //_Float16 b_jl = B[l*ldb + j]; // Column-major
                    _Float16 b_jl = B[j*ldb + l];   // Row-major
                    if (a_jl != 0.0F16 || b_jl != 0.0F16) {
                        _Float16 temp1 = alpha * b_jl;
                        _Float16 temp2 = alpha * a_jl;
                        for (int i = j; i < n; ++i) {
                            //C[j*ldc + i] += A[l*lda + i] * temp1 + B[l*ldb + i] * temp2; // Column-major
                            C[i*ldc + j] += A[i*lda + l] * temp1 + B[i*ldb + l] * temp2;   // Row-major
                        }
                    }
                }
            }
        }
    } else {  // Transposed case (TRANS = 'T' o 'C')
        if (upper) {
            for (int j = 0; j < n; ++j) {
                for (int i = 0; i <= j; ++i) {
                    _Float16 temp1 = 0.0F16, temp2 = 0.0F16;
                    for (int l = 0; l < k; ++l) {
                        //temp1 += A[i*lda + l] * B[j*ldb + l]; // Column-major
                        temp1 += A[j*lda + l] * B[i*ldb + l];   // Row-major
                        //temp2 += B[i*ldb + l] * A[j*lda + l]; // Column-major
                        temp2 += B[j*ldb + l] * A[i*lda + l];   // Row-major
                    }
                    // Conversión final a _Float16 con escalado
                    _Float16 acc = alpha * (temp1 + temp2);
                    if (beta == 0.0F16) {
                        //C[j*ldc + i] = acc; // Column-major
                        C[i*ldc + j] = acc;   // Row-major
                    } else {
                        //C[j*ldc + i] = beta * C[j*ldc + i] + acc; // Column-major
                        C[i*ldc + j] = beta * C[i*ldc + j] + acc;   // Row-major
                    }
                }
            }
        } else {
            for (int j = 0; j < n; ++j) {
                for (int i = j; i < n; ++i) {
                    _Float16 temp1 = 0.0F16, temp2 = 0.0F16;
                    for (int l = 0; l < k; ++l) {
                        //temp1 += A[i*lda + l] * B[j*ldb + l]; // Column-major
                        temp1 += A[j*lda + l] * B[i*ldb + l];   // Row-major
                        //temp2 += B[i*ldb + l] * A[j*lda + l]; // Column-major
                        temp2 += B[j*ldb + l] * A[i*lda + l];   // Row-major
                    }
                    
                    _Float16 acc = alpha * (temp1 + temp2);
                    if (beta == 0.0F16) {
                        //C[j*ldc + i] = acc; // Column-major
                        C[i*ldc + j] = acc;   // Row-major
                    } else {
                        //C[j*ldc + i] = beta * C[j*ldc + i] + acc; // Column-major
                        C[i*ldc + j] = beta * C[i*ldc + j] + acc;   // Row-major
                    }
                }
            }
        }
    }
}