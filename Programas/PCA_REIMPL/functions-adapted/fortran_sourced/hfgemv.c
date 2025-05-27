
#include "lapacke_utils_reimpl.h"

void hfgemv(char trans, int m, int n, lapack_float alpha, lapack_float *a, int lda,
           lapack_float *x, lapack_int incx, lapack_float beta, lapack_float *y, lapack_int incy) {
    
    // Constantes
    const lapack_float ZERO = (lapack_float)0.0;
    const lapack_float ONE = (lapack_float)1.0;

    // Local variables
    lapack_float temp;
    int i, j, ix, iy, jx, jy, kx, ky, lenx, leny;

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
        c_xerbla("HFGEMV", info);
        return;
    }

    // Quick return if possible
    if (m == 0 || n == 0 || (alpha == ZERO && beta == ONE)) {
        return;
    }

    // Set LENX and LENY (igual que antes)
    if(lsame_reimpl(trans, 'N')) {
        lenx = n;
        leny = m;
    } else {
        lenx = m;
        leny = n;
    }

    if(incx > 0) {
       kx = 0;
    } else {
        kx = (lenx - 1) * (-incx);
    }

    if(incy > 0) {
       ky = 0;
    } else {
        ky = (leny - 1) * (-incy);
    }

    // Handle beta scaling of Y
    if (beta != ONE) {
        if (incy == 1) {
            if (beta == ZERO) {
                for (i = 0; i < leny; i++) {
                    y[i] = ZERO;
                }
            } else {
                for (i = 0; i < leny; i++) {
                    y[i] *= beta;
                }
            }
        } else {
            iy = ky;
            if(beta == ZERO) {
                for (i = 0; i < leny; i++) {
                    y[iy] = ZERO;
                    iy += incy;
                }
            } else {
                for (i = 0; i < leny; i++) {
                    y[iy] *= beta;
                    iy += incy;
                }
            }
        }
    }

    if (alpha == ZERO) {
        return;
    }

    if (lsame_reimpl(trans, 'N')) {  // y = alpha*A*x + y
        jx = kx;
        if(incy == 1) {
            for(j = 0; j < n; j++) {
                temp = alpha * x[jx];
                for (i = 0; i < m; ++i) {
                    y[i] += temp * a[i + j * lda];
                }
                jx += incx;
            }
        } else {
            for(j = 0; j < n; j++) {
                temp = alpha * x[jx];
                iy = ky;
                for (i = 0; i < m; i++) {
                    y[iy] += temp * a[i + j * lda];
                    iy += incy;
                }
                jx += incx;
            }
        }
    } else {  // y = alpha*A^T*x + y
        jy = ky;
        if (incy == 1) {
            for (j = 0; j < n; j++) {
                temp = ZERO;
                for (i = 0; i < m; i++) {
                    temp += a[i + j * lda] * x[i];
                }
                y[jy] += alpha * temp;
                jy += incy;
            }
        } else {
            for (j = 0; j < n; j++) {
                temp = ZERO;
                ix = kx;
                for (i = 0; i < m; i++) {
                    temp += a[i + j * lda] * x[ix];
                    ix += incx;
                }
                y[jy] += alpha * temp;
                jy += incy;
            }
        }
    }
}