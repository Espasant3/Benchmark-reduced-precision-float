#include "../include/lapacke_utils_reimpl.h"

void hfsyr2(char uplo, lapack_int n, _Float16 alpha, const _Float16 *x, lapack_int incx,
           const _Float16 *y, lapack_int incy, _Float16 *a, lapack_int lda) {

    if (!lsame_reimpl(uplo, 'U') && !lsame_reimpl(uplo, 'L')) {
        c_xerbla("HFSYR2 ", 1);
        return;
    }
    if (n < 0) {
        c_xerbla("HFSYR2 ", 2);
        return;
    }
    if (incx == 0) {
        c_xerbla("HFSYR2 ", 5);
        return;
    }
    if (incy == 0) {
        c_xerbla("HFSYR2 ", 7);
        return;
    }
    if (lda < MAX(1, n)) {
        c_xerbla("HFSYR2 ", 9);
        return;
    }

    if (n == 0 || alpha == 0.0F16) {
        return;
    }

    int non_unit_inc = (incx != 1) || (incy != 1);
    lapack_int kx = 0, ky = 0, jx = 0, jy = 0;

    if (non_unit_inc) {
        kx = (incx > 0) ? 0 : (n - 1) * ABS(incx);
        ky = (incy > 0) ? 0 : (n - 1) * ABS(incy);
        jx = kx;
        jy = ky;
    }

    if (lsame_reimpl(uplo, 'U')) {
        if (!non_unit_inc) {
            for (lapack_int j = 0; j < n; ++j) {
                _Float16 xj = x[j], yj = y[j];
                if (xj == 0.0f && yj == 0.0f) continue;
                _Float16 tmp1 = yj * alpha;
                _Float16 tmp2 = xj * alpha;
                for (lapack_int i = 0; i <= j; ++i) {
                    a[i + j * lda] += x[i] * tmp1 + y[i] * tmp2;
                }
            }
        } else {
            for (lapack_int j = 0; j < n; ++j) {
                _Float16 x_val = x[jx], y_val = y[jy];
                if (x_val != 0.0f || y_val != 0.0f) {
                    _Float16 tmp1 = y_val * alpha;
                    _Float16 tmp2 = x_val * alpha;
                    lapack_int ix = kx, iy = ky;
                    for (lapack_int i = 0; i <= j; ++i) {
                        a[i + j * lda] += x[ix] * tmp1 + y[iy] * tmp2;
                        ix += incx;
                        iy += incy;
                    }
                }
                jx += incx;
                jy += incy;
            }
        }
    } else {
        if (!non_unit_inc) {
            for (lapack_int j = 0; j < n; ++j) {
                _Float16 xj = x[j], yj = y[j];
                if (xj == 0.0f && yj == 0.0f) continue;
                _Float16 tmp1 = yj * alpha;
                _Float16 tmp2 = xj * alpha;
                for (lapack_int i = j; i < n; ++i) {
                    a[i + j * lda] += x[i] * tmp1 + y[i] * tmp2;
                }
            }
        } else {
            for (lapack_int j = 0; j < n; ++j) {
                _Float16 x_val = x[jx], y_val = y[jy];
                if (x_val != 0.0f || y_val != 0.0f) {
                    _Float16 tmp1 = y_val * alpha;
                    _Float16 tmp2 = x_val * alpha;
                    lapack_int ix = jx, iy = jy;
                    for (lapack_int i = j; i < n; ++i) {
                        a[i + j * lda] += x[ix] * tmp1 + y[iy] * tmp2;
                        ix += incx;
                        iy += incy;
                    }
                }
                jx += incx;
                jy += incy;
            }
        }
    }
}