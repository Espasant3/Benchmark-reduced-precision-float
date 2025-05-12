
#include "../include/lapacke_utils_reimpl.h"

void hfsymv(char uplo, lapack_int n, _Float16 alpha, _Float16 *A, lapack_int lda,
           _Float16 *x, lapack_int incx, _Float16 beta, _Float16 *y, lapack_int incy) {

    lapack_int info = 0;
    if (!lsame_reimpl(uplo, 'U') && !lsame_reimpl(uplo, 'L')) {
        info = 1;
    } else if (n < 0) {
        info = 2;
    } else if (lda < MAX(1, n)) {
        info = 5;
    } else if (incx == 0) {
        info = 7;
    } else if (incy == 0) {
        info = 10;
    }
    if (info != 0) {
        c_xerbla("ssymv ", info);
        return;
    }

    if (n == 0 || (alpha == 0.0F16 && beta == 1.0F16)) {
        return;
    }

    // Escalado de Y con beta (operaciones directas en _Float16)
    if (beta != 1.0F16) {
        if (incy == 1) {
            if (beta == 0.0F16) {
                for (lapack_int i = 0; i < n; i++) {
                    y[i] = 0.0F16;
                }
            } else {
                for (lapack_int i = 0; i < n; i++) {
                    y[i] *= beta;  // Escalado directo
                }
            }
        } else {
            lapack_int iy = (incy > 0) ? 0 : ((n - 1) * ABS(incy));
            if (beta == 0.0F16) {
                for (lapack_int i = 0; i < n; i++) {
                    y[iy] = 0.0F16;
                    iy += incy;
                }
            } else {
                for (lapack_int i = 0; i < n; i++) {
                    y[iy] *= beta;  // Escalado directo
                    iy += incy;
                }
            }
        }
    }

    if (alpha == 0.0F16) {
        return;
    }

    // Cálculo principal sin conversiones intermedias
    if (lsame_reimpl(uplo, 'U')) {
        if (incx == 1 && incy == 1) {
            for (lapack_int j = 0; j < n; j++) {
                _Float16 xj = x[j];                // Eliminada conversión a float
                _Float16 temp1 = alpha * xj;        // Operación directa en _Float16
                _Float16 temp2 = 0.0F16;

                // Parte triangular superior
                for (lapack_int i = 0; i < j; i++) {
                    _Float16 A_ij = A[i + j * lda];
                    y[i] += temp1 * A_ij;
                    temp2 += A_ij * x[i];
                }

                // Diagonal
                y[j] += temp1 * A[j + j * lda] + alpha * temp2;
            }
        } else {
            lapack_int kx = (incx > 0) ? 0 : ((n - 1) * ABS(incx));
            lapack_int ky = (incy > 0) ? 0 : ((n - 1) * ABS(incy));
            lapack_int jx = kx;
            lapack_int jy = ky;

            for (lapack_int j = 0; j < n; j++) {
                _Float16 xj = x[jx];
                _Float16 temp1 = alpha * xj;
                _Float16 temp2 = 0.0F16;
                lapack_int ix = kx;
                lapack_int iy = ky;

                for (lapack_int i = 0; i < j; i++) {
                    _Float16 A_ij = A[i + j * lda];
                    y[iy] += temp1 * A_ij;
                    temp2 += A_ij * x[ix];
                    ix += incx;
                    iy += incy;
                }

                y[jy] += temp1 * A[j + j * lda] + alpha * temp2;
                jx += incx;
                jy += incy;
            }
        }
    } else {
        if (incx == 1 && incy == 1) {
            for (lapack_int j = 0; j < n; j++) {
                _Float16 temp1 = alpha * x[j];      // Conversión eliminada
                _Float16 temp2 = 0.0F16;

                // Diagonal
                y[j] += temp1 * A[j + j * lda];

                // Parte triangular inferior
                for (lapack_int i = j + 1; i < n; i++) {
                    _Float16 A_ij = A[i + j * lda];
                    y[i] += temp1 * A_ij;
                    temp2 += A_ij * x[i];
                }

                y[j] += alpha * temp2;
            }
        } else {
            lapack_int kx = (incx > 0) ? 0 : ((n - 1) * ABS(incx));
            lapack_int ky = (incy > 0) ? 0 : ((n - 1) * ABS(incy));
            lapack_int jx = kx;
            lapack_int jy = ky;

            for (lapack_int j = 0; j < n; j++) {
                _Float16 temp1 = alpha * x[jx];
                _Float16 temp2 = 0.0F16;

                // Diagonal
                y[jy] += temp1 * A[j + j * lda];

                lapack_int ix = jx + incx;
                lapack_int iy_val = jy + incy;

                // Parte triangular inferior
                for (lapack_int i = j + 1; i < n; i++) {
                    _Float16 A_ij = A[i + j * lda];
                    y[iy_val] += temp1 * A_ij;
                    temp2 += A_ij * x[ix];
                    ix += incx;
                    iy_val += incy;
                }

                y[jy] += alpha * temp2;
                jx += incx;
                jy += incy;
            }
        }
    }
}