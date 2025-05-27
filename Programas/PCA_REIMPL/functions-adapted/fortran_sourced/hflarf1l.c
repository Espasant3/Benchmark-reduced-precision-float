
#include "lapacke_utils_reimpl.h"

void hflarf1l(char side, int m, int n, lapack_float *v, int incv, lapack_float tau, lapack_float *C, int ldc, lapack_float *work){

    // Constantes
    lapack_float ZERO = (lapack_float)0.0;
    lapack_float ONE = (lapack_float)1.0;

    int applyleft, lastv, lastc, firstv, i;

    applyleft = lsame_reimpl(side, 'L');

    firstv = 1;
    lastc = 0;

    if(tau != ZERO){
        lastv = applyleft ? m : n;

        i = 0;
        
        while(lastv > firstv && v[i] == ZERO){
            firstv++;
            i += incv;
        }
        if(applyleft){
            lastc = ilaslc_reimpl_half_precision(lastv, n, C, ldc) + 1; // +1 para incluir la fila 0
        }else{
            lastc = ilaslr_reimpl_half_precision(m, lastv, C, ldc) + 1; // +1 para incluir la columna 0
        }

    }

    if(lastc == 0) return;

    if(applyleft){

        if(lastv == firstv){
            hfscal(lastc, ONE - tau, &C[lastv - 1], ldc);
        }else{
            // w = C(2:lastv,1:lastc)^T * v(2:lastv)
            hfgemv('T', lastv - firstv, lastc, ONE, &C[firstv - 1], ldc,
                   &v[i], incv, ZERO, work, 1);

            // w += v[0] * C[0]
            hfaxpy(lastc, ONE, &C[lastv - 1], ldc, work, 1);

            // C = C - tau * v * w^T
            hfaxpy(lastc, -tau, work, 1, &C[lastv - 1], ldc);

            hfger(lastv - firstv, lastc, -tau, &v[i], incv,
                  work, 1, &C[firstv - 1], ldc);
        }
    } else {
        if(lastv == firstv){
            hfscal(lastc, ONE - tau, &C[(lastv - 1) * ldc], 1);
        }else{
            // w = C(1:lastc,2:lastv) * v(2:lastv)
            hfgemv('N', lastc, lastv - firstv, ONE, &C[(firstv - 1) * ldc], ldc,
                   &v[i], incv, ZERO, work, 1);

            // w += v[0] * C[0]
            hfaxpy(lastc, ONE, &C[(lastv - 1) * ldc], 1, work, 1);

            // C = C - tau * w * v^T
            hfaxpy(lastc, -tau, work, 1, &C[(lastv - 1) * ldc], 1);

            hfger(lastc, lastv - firstv, -tau, work, 1,
                  &v[i], incv, &C[(firstv - 1) * ldc], ldc);
        }
    }

}