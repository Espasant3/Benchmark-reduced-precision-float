
#include "../include/lapacke_utils_reimpl.h"
/*
void hfaxpy(int n, _Float16 alpha, _Float16 *x, int incx, _Float16 *y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] += alpha * x[i * incx];
    }
}
*/
void hflarf1f(char side, int m, int n, _Float16 *v, int incv, _Float16 tau,
             _Float16 *C, int ldc, _Float16 *work)
{
    const _Float16 ONE = 1.0F16, ZERO = 0.0F16;
    int applyleft, lastv, lastc;
    int one = 1;
    char trans;
    _Float16 one_minus_tau;

    /* Verificar lado de la operación */
    applyleft = lsame_reimpl(side, 'L');

    lastv = 0;
    if (tau != ZERO) {
        lastv = applyleft ? m : n;
        int i;
        
        if (incv > 0) {
            i = (lastv - 1) * incv;  // Último elemento posible
        } else {
            i = (1 - lastv) * incv;  // Manejo de INCV negativo
        }
        
        // Detenerse cuando LASTV > 1 como en Fortran
        while (lastv > 1 && v[i] == ZERO) {
            lastv--;
            i -= incv;
        }
    }

    if (tau != ZERO) {
        if (applyleft) {
            lastc = ilaslc_reimpl_hf(lastv, n, C, ldc) + 1; // +1 para incluir la fila 0
        } else {
            lastc = ilaslr_reimpl_hf(m, lastv, C, ldc) + 1; // +1 para incluir la columna 0
        }
    } else {
        lastc = 0;
    }
    
    if (lastc == 0) return;

    if (applyleft) {
        if (lastv == 1) {
            one_minus_tau = ONE - tau;
            hfscal(lastc, one_minus_tau, C, ldc);
        } else {
            /* w = C(2:lastv,1:lastc)^T * v(2:lastv) */
            trans = 'T';
            int mv_rows = lastv-1;
            hfgemv(trans, mv_rows, lastc, ONE, &C[1], ldc,
                   &v[incv], incv, ZERO, work, one);

            /* w += v[0] * C[0] */
            hfaxpy(lastc, v[0], C, ldc, work, one);

            /* C = C - tau * v * w^T */
            _Float16 neg_tau = -tau;
            hfaxpy(lastc, neg_tau, work, one, C, ldc);
            hfger(mv_rows, lastc, neg_tau, &v[incv], incv,
                  work, one, &C[1], ldc);
        }
    } else {
        if (lastv == 1) {
            one_minus_tau = ONE - tau;
            hfscal(lastc, one_minus_tau, C, one);
        } else {
            /* w = C(1:lastc,2:lastv) * v(2:lastv) */
            trans = 'N';
            int mv_cols = lastv-1;
            hfgemv(trans, lastc, mv_cols, ONE, &C[ldc], ldc,
                   &v[incv], incv, ZERO, work, one);

            /* w += v[0] * C[0] */
            hfaxpy(lastc, v[0], C, one, work, one);

            /* C = C - tau * w * v^T */
            _Float16 neg_tau = -tau;
            hfaxpy(lastc, neg_tau, work, one, C, one);
            hfger(lastc, mv_cols, neg_tau, work, one,
                  &v[incv], incv, &C[ldc], ldc);
        }
    }
}