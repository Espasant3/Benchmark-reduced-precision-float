
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file hflartg.c
 * \brief Genera una rotación de Givens en _Float16
 */

/**
 * \brief Calcula los parámetros de una rotación de Givens en _Float16
 * 
 * \details Versión _Float16 de LAPACK's SLARTG. Calcula valores c, s, r tales que:
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
 * - custom_sqrtf16 debe ser precisa en rango [0, FP16_MAX]
 * - La precisión de _Float16 puede afectar la ortogonalidad para valores extremos
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
 * _Float16 c, s, r;
 * hflartg(3.0F16, 4.0F16, &c, &s, &r);
 * // c ≈ 0.6F16, s ≈ 0.8F16, r = 5.0F16
 * 
 * \see hflamch Para obtención de constantes máquina
 * \see custom_sqrtf16 Implementación de raíz cuadrada en _Float16
 * \see ABS_Float16 Macro para valor absoluto en _Float16
 */

void hflartg(_Float16 f, _Float16 g, _Float16 *c, _Float16 *s, _Float16 *r) {
    _Float16 safmin = hflamch('S');
    _Float16 safmax = 1.0F16 / safmin;
    
    _Float16 f1 = ABS_Float16(f);
    _Float16 g1 = ABS_Float16(g);

    if (g == 0.0F16) {
        *c = 1.0F16;
        *s = 0.0F16;
        *r = f;
        return;
    } else if (f == 0.0F16) {
        *c = 0.0F16;
        *s = (g > 0.0F16) ? 1.0F16 : -1.0F16;
        *r = g1;
        return;
    } else {
        _Float16 rtmin = custom_sqrtf16(safmin);
        _Float16 rtmax = custom_sqrtf16(safmax / 2.0F16);

        if (f1 > rtmin && f1 < rtmax && g1 > rtmin && g1 < rtmax) {
            _Float16 d = custom_sqrtf16(f * f + g * g);
            *c = f1 / d;
            *r = (f >= 0.0F16) ? d : -d;
            *s = g / (*r);
        } else {
            // Calcular u como min(safmax, max(safmin, f1, g1))
            _Float16 u = MAX(f1, g1);
            u = MAX(u, safmin);
            u = MIN(u, safmax);

            _Float16 fs = f / u;
            _Float16 gs = g / u;
            _Float16 d = custom_sqrtf16(fs * fs + gs * gs);
            *c = ABS_Float16(fs) / d;
            *r = (f >= 0.0F16) ? d : -d;
            *s = gs / (*r);
            *r *= u;
        }
    }
}