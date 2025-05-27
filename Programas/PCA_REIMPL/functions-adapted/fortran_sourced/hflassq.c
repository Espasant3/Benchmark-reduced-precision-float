
#include "lapacke_utils_reimpl.h"

void hflassq(int n, const lapack_float *x, int incx, lapack_float *scale, lapack_float *sumsq) {
    if (n <= 0 || LAPACK_HFISNAN(*scale) || LAPACK_HFISNAN(*sumsq) || *sumsq < 0.0F16) return;

    float scale_f = (float)(*scale);
    float sumsq_f = (float)(*sumsq);
    int ix = (incx >= 0) ? 0 : (1 - n) * incx;

    for (int i = 0; i < n; ++i) {
        float xi = fabsf((float)x[ix]);
        if (xi != 0.0f) {
            if (scale_f < xi) {
                float ratio = scale_f / xi;
                sumsq_f = 1.0f + sumsq_f * ratio * ratio;
                scale_f = xi;
            } else {
                float ratio = xi / scale_f;
                sumsq_f += ratio * ratio;
            }
            // Saturación intermedia para evitar overflow
            if (sumsq_f > FP16_MAX) {
                sumsq_f = FP16_MAX;
                scale_f = 1.0f;
                break;  // Salir del bucle si ya se alcanzó el máximo
            }
        }
        ix += incx;
    }

    // Saturación final
    if (sumsq_f > FP16_MAX) {
        sumsq_f = FP16_MAX;
        scale_f = 1.0f;
    }

    *scale = (lapack_float)scale_f;
    *sumsq = (lapack_float)sumsq_f;
}