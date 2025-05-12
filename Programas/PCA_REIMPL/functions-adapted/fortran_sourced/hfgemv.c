
#include "../include/lapacke_utils_reimpl.h"

void hfgemv(char trans, int m, int n, _Float16 alpha, _Float16 *a, int lda,
           _Float16 *x, lapack_int incx, _Float16 beta, _Float16 *y, lapack_int incy) {
    // Parameters
    const _Float16 one = 1.0F16, zero = 0.0F16;

    // Local variables
    _Float16 temp;
    int i, j, ix, iy, jx, jy, kx, ky, lenx, leny;
    lapack_logical transposed;

    // Check for valid TRANS parameter
    transposed = !lsame_reimpl(trans, 'N');

    // Test input parameters (igual que antes)
    int info = 0;
    if (!lsame_reimpl(trans, 'N') && !lsame_reimpl(trans, 'T') && !lsame_reimpl(trans, 'C')) {
        info = 1;
    } else if (m < 0) {
        info = 2;
    } else if (n < 0) {
        info = 3;
    } else if (lda < MAX(1, m)) { 
        info = 6;
    } else if (incx == 0) {
        info = 8;
    } else if (incy == 0) {
        info = 11;
    }

    if (info != 0) {
        c_xerbla("HFGEMV ", info);
        return;
    }

    // Quick return if possible
    if (m == 0 || n == 0 || (alpha == zero && beta == one)) {
        return;
    }

    // Set LENX and LENY (igual que antes)
    lenx = transposed ? m : n;
    leny = transposed ? n : m;

    // Compute start points for X and Y (igual que antes)
    kx = (incx > 0) ? 0 : (lenx - 1) * ABS(incx);
    ky = (incy > 0) ? 0 : (leny - 1) * ABS(incy);

    // Handle beta scaling of Y (igual que antes, con _Float16)
    if (beta != one) {
        if (incy == 1) {
            if (beta == zero) {
                for (i = 0; i < leny; ++i) {
                    y[i] = zero;
                }
            } else {
                for (i = 0; i < leny; ++i) {
                    y[i] *= beta;
                }
            }
        } else {
            iy = ky;
            for (i = 0; i < leny; ++i) {
                if (beta == zero) {
                    y[iy] = zero;
                } else {
                    y[iy] *= beta;
                }
                iy += incy;
            }
        }
    }

    if (alpha == zero) {
        return;
    }

    // Perform matrix-vector multiplication (igual que antes, con _Float16)
    if (!transposed) {  // y = alpha*A*x + y
        jx = kx;
        for (j = 0; j < n; ++j) {
            temp = alpha * x[jx];
            if (temp != zero) {
                if (incy == 1) {
                    for (i = 0; i < m; ++i) {
                        y[i] += temp * a[i + j * lda];
                    }
                } else {
                    iy = ky;
                    for (i = 0; i < m; ++i) {
                        y[iy] += temp * a[i + j * lda];
                        iy += incy;
                    }
                }
            }
            jx += incx;
        }
    } else {  // y = alpha*A^T*x + y
        jy = ky;
        for (j = 0; j < n; ++j) {
            temp = zero;
            ix = kx;
            for (i = 0; i < m; ++i) {
                temp += a[i + j * lda] * x[ix];
                ix += incx;
            }
            y[jy] += alpha * temp;
            jy += incy;
        }
    }
}