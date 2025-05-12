
#include "../include/lapacke_utils_reimpl.h"

// Umbrales específicos para _Float16
#define TSMALL (custom_sqrtf16(FP16_MIN) * 3.0F16)
#define TBIG   (1.0F16 / (3.0F16 * custom_sqrtf16(FP16_MAX)))
#define SSMALL (1.0F16 / TSMALL)
#define SBIG   (1.0F16 / TBIG)


void hflassq(int n, const _Float16 *x, int incx, _Float16 *scale, _Float16 *sumsq) {
    if (n <= 0 || LAPACK_HFISNAN(*scale) || LAPACK_HFISNAN(*sumsq) || *sumsq < 0.0F16) return;

    // Inicialización equivalente adaptada
    if (*sumsq == 0.0F16) *scale = 1.0F16;
    if (*scale == 0.0F16) {
        *scale = 1.0F16;
        *sumsq = 0.0F16;
    }

    _Float16 abig = 0.0F16, amed = 0.0F16, asml = 0.0F16;
    bool notbig = true;
    int ix = (incx == 0) ? 0 : ((incx > 0) ? 0 : (1 - n) * incx);

    // Procesamiento principal optimizado para media precisión
    for (int i = 0; i < n; ++i) {
        _Float16 ax = ABS_Float16(x[ix]);
        
        if (ax > TBIG) {
            abig += (ax * SBIG) * (ax * SBIG);
            notbig = false;
        } else if (ax < TSMALL && notbig) {
            asml += (ax * SSMALL) * (ax * SSMALL);
        } else {
            amed += ax * ax;
        }
        
        if (incx != 0) ix += incx;
    }

    // Integración del sumsq existente con precisión controlada
    if (*sumsq > 0.0F16) {
        _Float16 ax = *scale * custom_sqrtf16(*sumsq);
        
        if (ax > TBIG) {
            _Float16 scaled_scale = (*scale > 1.0F16) ? *scale * SBIG : *scale;
            abig += scaled_scale * scaled_scale * *sumsq;
        } else if (ax < TSMALL && notbig) {
            _Float16 scaled_scale = (*scale < 1.0F16) ? *scale * SSMALL : *scale;
            asml += scaled_scale * scaled_scale * *sumsq;
        } else {
            amed += *scale * *scale * *sumsq;
        }
    }

    // Combinación final optimizada para media precisión
    if (abig > 0.0F16) {
        *scale = 1.0F16 / SBIG;
        *sumsq = abig + (amed * SBIG) * SBIG;
    } else if (asml > 0.0F16) {
        if (amed > 0.0F16 || LAPACK_HFISNAN(amed)) {
            _Float16 ymed = custom_sqrtf16(amed);
            _Float16 ysml = custom_sqrtf16(asml) * SSMALL;
            
            *scale = 1.0F16;
            *sumsq = (ysml > ymed) ? 
                ysml*ysml * (1.0F16 + (ymed/ysml)*(ymed/ysml)) :
                ymed*ymed * (1.0F16 + (ysml/ymed)*(ysml/ymed));
        } else {
            *scale = SSMALL;
            *sumsq = asml;
        }
    } else {
        *scale = 1.0F16;
        *sumsq = amed;
    }
}