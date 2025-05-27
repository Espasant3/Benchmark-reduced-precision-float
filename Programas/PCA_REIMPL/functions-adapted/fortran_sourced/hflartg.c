
#include "lapacke_utils_reimpl.h"

/**
 * \file hflartg.c
 * \brief Genera una rotación de Givens en lapack_float
 */

/**
 * \brief Calcula los parámetros de una rotación de Givens en lapack_float
 * 
 * \details Versión lapack_float de LAPACK's SLARTG. Calcula valores c, s, r tales que:
 *          \f[
 *          \begin{pmatrix} c & s \\ -s & c \end{pmatrix}^T \begin{pmatrix} f \\ g \end{pmatrix} = \begin{pmatrix} r \\ 0 \end{pmatrix}
 *          \f]
 *          Implementación numéricamente estable con escalado para evitar underflow/overflow.
 * 
 * \param[in] f    Primer elemento del vector
 * \param[in] g    Segundo elemento del vector
 * \param[out] c   Coseno del ángulo de rotación
 * \param[out] s   Seno del ángulo de rotación
 * \param[out] r   Valor escalado resultante: \f$ r = \sqrt{f^2 + g^2} \f$
 * 
 * \note
 * - Mantiene relación: \f$ c^2 + s^2 = 1 \f$
 * - Para precisión mejorada, escala entradas cuando están fuera de [rtmin, rtmax]
 * - rtmin = \f$ \sqrt{\text{FP16\_TRUE\_MIN}} \f$, rtmax = \f$ \sqrt{\text{FP16\_MAX}/2} \f$
 * 
 * \warning
 * - No maneja NaN/Inf en f o g (comportamiento indefinido)
 * - custom_sqrtf_half_precision debe ser precisa en rango [0, FP16_MAX]
 * - La precisión de lapack_float puede afectar la ortogonalidad para valores extremos
 * 
 * \par Algoritmo:
 * 1. Casos triviales (g=0 o f=0)
 * 2. Si f y g están en rango seguro:
 *    \f[ d = \sqrt{f^2 + g^2},\quad c = |f|/d,\quad r = \text{sign}(f) \cdot d \f]
 * 3. Si no:
 *    \f[ u = \text{scale}(f, g),\quad d = \sqrt{(f/u)^2 + (g/u)^2} \f]
 *    \f[ r = \text{sign}(f) \cdot d \cdot u \f]
 * 
 * \example
 * lapack_float c, s, r;
 * hflartg(3.0F16, 4.0F16, &c, &s, &r);
 * // c ≈ 0.6F16, s ≈ 0.8F16, r = 5.0F16
 * 
 * \see hflamch_half_precision Para obtención de constantes máquina
 * \see custom_sqrtf_half_precision Implementación de raíz cuadrada en _Float16
 * \see ABS_half_precision Macro para valor absoluto en lapack_float
 */

void hflartg(lapack_float f, lapack_float g, lapack_float *c, lapack_float *s, lapack_float *r) {
    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;

    lapack_float safmin = hflamch_half_precision('S');
    lapack_float safmax = ONE / safmin;
    
    lapack_float f1 = ABS_half_precision(f);
    lapack_float g1 = ABS_half_precision(g);

    if (g == ZERO) {
        *c = ONE;
        *s = ZERO;
        *r = f;
        return;
    } else if (f == ZERO) {
        *c = ZERO;
        *s = (g > ZERO) ? ONE : -ONE;
        *r = g1;
        return;
    } else {
        lapack_float rtmin = custom_sqrtf_half_precision(safmin);
        lapack_float rtmax = custom_sqrtf_half_precision(safmax / 2.0F16);

        if (f1 > rtmin && f1 < rtmax && g1 > rtmin && g1 < rtmax) {
            // Cálculo estable de la hipotenusa usando el método de LAPACK
            lapack_float a = MAX(f1, g1);
            lapack_float b = MIN(f1, g1);
            lapack_float ratio = (a == ZERO) ? ZERO : b / a;
            lapack_float d = a * custom_sqrtf_half_precision(ONE + ratio * ratio);
            *c = (a == ZERO) ? ONE : f1 / d;
            *r = (f >= ZERO) ? d : -d;
            *s = g / (*r);
        } else {
            // Escalar f y g para evitar underflow/overflow
            lapack_float u = MAX(f1, g1);
            u = MAX(u, rtmin);  // Asegurar que u ≥ rtmin
            u = MIN(u, safmax);

            lapack_float fs = f / u;
            lapack_float gs = g / u;

            // Cálculo estable de la hipotenusa para fs y gs
            lapack_float a = MAX(ABS_half_precision(fs), ABS_half_precision(gs));
            lapack_float b = MIN(ABS_half_precision(fs), ABS_half_precision(gs));
            lapack_float ratio = (a == ZERO) ? ZERO : b / a;
            lapack_float d = a * custom_sqrtf_half_precision(ONE + ratio * ratio);

            *c = (a == ZERO) ? ONE : ABS_half_precision(fs) / d;
            *r = (f >= ZERO) ? d : -d;
            *s = gs / (*r);
            *r *= u;  // Re-escalar r
        }
    }
}