
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file hflapy2.c
 * \brief Cálculo seguro de la norma euclídea en _Float16
 */

/**
 * \brief Calcula sqrt(x² + y²) evitando desbordamientos en _Float16
 * 
 * \details Versión _Float16 de LAPACK's SLAPY2. Implementa:
 *          - Manejo de NaN: Si x o y es NaN, retorna inmediatamente NaN
 *          - Evita overflow mediante escalado: sqrt(w²*(1 + (z/w)²)) donde w = max(|x|, |y|)
 *          - Precisión mejorada para valores pequeños
 * 
 * \param[in] x Primer valor _Float16
 * \param[in] y Segundo valor _Float16
 * \return _Float16 Resultado de sqrt(x² + y²)
 * 
 * \note
 * - Implementación numéricamente estable:
 *   \f[
 *   \text{result} = \begin{cases}
 *   w\sqrt{1 + (z/w)^2} & \text{si } w \neq 0 \\
 *   0 & \text{si } w = 0
 *   \end{cases}
 *   \f]
 * - Mayor precisión que cálculo directo para |x| ≫ |y| o viceversa
 * 
 * \warning
 * - Requiere que custom_sqrtf16 maneje entradas en rango [1, 2]
 * - No verifica overflow en custom_sqrtf16 (asume w <= FP16_MAX)
 * 
 * \par Algoritmo:
 * 1. Detección temprana de NaN
 * 2. Cálculo de valores absolutos
 * 3. Escalado con w = max(|x|, |y|)
 * 4. Cálculo seguro de 1 + (z/w)²
 * 
 * \example
 * _Float16 norm = hflapy2(3.0F16, 4.0F16); // 5.0F16
 * _Float16 nan_res = hflapy2(NAN_Float16, 1.0F16); // NAN_Float16
 * 
 * \see custom_sqrtf16 Para implementación de raíz cuadrada en _Float16
 * \see LAPACK_HFISNAN Macro para detección de NaN en _Float16
 * \see FP16_MAX Valor máximo representable en _Float16
 */

_Float16 hflapy2(_Float16 x, _Float16 y) {
    if (LAPACK_HFISNAN(x)) {
        return x;
    }
    if (LAPACK_HFISNAN(y)) {
        return y;
    }

    _Float16 xabs = ABS_Float16(x);
    _Float16 yabs = ABS_Float16(y);
    _Float16 w = MAX(xabs, yabs);
    _Float16 z = MIN(xabs, yabs);

    if (z == 0.0F16 || w > FP16_MAX) {
        return w;
    } else {
        _Float16 ratio = z / w;
        return w * custom_sqrtf16(1.0F16 + ratio * ratio);
    }
}