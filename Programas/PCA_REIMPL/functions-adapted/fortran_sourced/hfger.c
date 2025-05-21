
#include "../include/lapacke_utils_reimpl.h"

void hfger(int m, int n, _Float16 alpha, const _Float16 *x, int incx, 
         const _Float16 *y, int incy, _Float16 *a, int lda) 
{
    int info = 0;
    if (m < 0) {
        info = 1;
    } else if (n < 0) {
        info = 2;
    } else if (incx == 0) {
        info = 5;
    } else if (incy == 0) {
        info = 7;
    } else if (lda < MAX(1, m)) {
        info = 9;
    }
    
    if (info != 0) {
        c_xerbla("HFGER", info);
        return;
    }

    // Cambiar constante 0.0f por conversión explícita
    if (m == 0 || n == 0 || alpha == 0.0F16) {
        return;
    }

    int jy = (incy > 0) ? 0 : (n - 1) * (-incy);

    if (incx == 1) {
        for (int j = 0; j < n; j++) {
            const _Float16 y_val = y[jy];
            if (y_val != 0.0F16) {
                const _Float16 temp = alpha * y_val;
                for (int i = 0; i < m; i++) {
                    a[i + j * lda] += x[i] * temp;
                }
            }
            jy += incy;
        }
    } else {
        const int kx = (incx > 0) ? 0 : (m - 1) * (-incx);
        
        for (int j = 0; j < n; j++) {
            const _Float16 y_val = y[jy];
            if (y_val != 0.0F16) {
                const _Float16 temp = alpha * y_val;
                int ix = kx;
                for (int i = 0; i < m; i++) {
                    a[i + j * lda] += x[ix] * temp;
                    ix += incx;
                }
            }
            jy += incy;
        }
    }
}