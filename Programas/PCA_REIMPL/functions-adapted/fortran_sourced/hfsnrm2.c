
#include "../include/lapacke_utils_reimpl.h" //Se puede evitar poner la ruta completa si se configura el compilador con algo como gcc -I./include -o programa utils/lapacke_ssy_nancheck_reimpl.c


//Opera todo en float y al final convierte a _Float16
_Float16 hfnrm2(int n, _Float16 *x, int incx) { 
    if (n <= 0) {
        return 0.0F16;
    }

    const float tsml = powf(FP16_RADIX, ceilf((FP16_MIN_EXP - 1) * 0.5f));
    const float tbig = powf(FP16_RADIX, floorf((FP16_MAX_EXP - FP16_MANT_DIG + 1) * 0.5f));
    const float ssml = powf(FP16_RADIX, -floorf((FP16_MIN_EXP - FP16_MANT_DIG) * 0.5f));
    const float sbig = powf(FP16_RADIX, -ceilf((FP16_MAX_EXP + FP16_MANT_DIG - 1) * 0.5f));
    const float maxN = powf(2, FP16_MAX_EXP) * (1 - powf(2, -FP16_MANT_DIG));

    float scl = 1.0f;
    float sumsq = 0.0f;
    float abig = 0.0f, amed = 0.0f, asml = 0.0f;
    bool notbig = true;

    int ix = (incx > 0) ? 0 : - (n - 1) * incx;

    for (int i = 0; i < n; ++i) {
        float ax = fabsf((float)x[ix]); // Convertir a float para cálculos
        if (ax > tbig) {
            abig += (ax * sbig) * (ax * sbig);
            notbig = false;
        } else if (ax < tsml) {
            if (notbig) {
                asml += (ax * ssml) * (ax * ssml);
            }
        } else {
            amed += ax * ax;
        }
        ix += incx;
    }

    if (abig > 0.0f) {
        if (amed > 0.0f || amed > maxN || isnan(amed)) {
            abig += (amed * sbig) * sbig;
        }
        scl = 1.0f / sbig;
        sumsq = abig;
    } else if (asml > 0.0f) {
        if (amed > 0.0f || amed > maxN || isnan(amed)) {
            float ymed = sqrtf(amed);
            float ysml = sqrtf(asml) / ssml;
            float ymax = (ysml > ymed) ? ysml : ymed;
            float ymin = (ysml > ymed) ? ymed : ysml;
            scl = 1.0f;
            sumsq = ymax * ymax * (1.0f + (ymin / ymax) * (ymin / ymax));
        } else {
            scl = 1.0f / ssml;
            sumsq = asml;
        }
    } else {
        scl = 1.0f;
        sumsq = amed;
    }

    return (_Float16)(scl * sqrtf(sumsq)); // Cast final a _Float16
}