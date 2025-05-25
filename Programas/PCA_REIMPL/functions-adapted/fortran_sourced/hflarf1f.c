
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
    int applyleft, lastv, lastc;

    /* Verificar lado de la operación */
    applyleft = lsame_reimpl(side, 'L');

    lastv = 1;
    lastc = 0;
    if (tau != 0.0F16) {
        lastv = applyleft ? m : n;
        int i;
        
        if (incv > 0) {
            i = (lastv - 1) * incv;  // Último elemento posible
        } else {
            i = 0;  // Manejo de INCV negativo
        }
        
        // Detenerse cuando LASTV > 1 como en Fortran
        while(lastv > 1 && v[i] == 0.0F16){
            lastv--;
            i -= incv;
        }

        if(applyleft){
            lastc = ilaslc_reimpl_Float16(lastv, n, C, ldc) + 1; // +1 para incluir la fila 0
        }else{
            lastc = ilaslr_reimpl_Float16(m, lastv, C, ldc) + 1; // +1 para incluir la columna 0
            
        }

    }
    if (lastc == 0) return;

    if (applyleft) {
        if (lastv == 1) {
            hfscal(lastc, 1.0F16 - tau, C, ldc);
        } else {
            /* w = C(2:lastv,1:lastc)^T * v(2:lastv) */
            hfgemv('T', lastv-1, lastc, 1.0F16, &C[1], ldc,
                   &v[incv], incv, 0.0F16, work, 1);

            /* w += v[0] * C[0] */
            hfaxpy(lastc, 1.0F16, C, ldc, work, 1);

            /* C = C - tau * v * w^T */
            hfaxpy(lastc, -tau, work, 1, C, ldc);

            hfger(lastv-1, lastc, -tau, &v[incv], incv,
                  work, 1, &C[1], ldc);
        }
    } else {
        if (lastv == 1) {
            hfscal(lastc, 1.0F16 - tau, C, 1);
        } else {
            /* w = C(1:lastc,2:lastv) * v(2:lastv) */
            hfgemv('N', lastc, lastv-1, 1.0F16, &C[ldc], ldc,
                   &v[incv], incv, 0.0F16, work, 1);

            /* w += v[0] * C[0] */
            hfaxpy(lastc, 1, C, 1, work, 1);

            /* C = C - tau * w * v^T */
            hfaxpy(lastc, -tau, work, 1, C, 1);

            hfger(lastc, lastv-1, -tau, work, 1,
                  &v[incv], incv, &C[ldc], ldc);
        }
    }
    return;
}