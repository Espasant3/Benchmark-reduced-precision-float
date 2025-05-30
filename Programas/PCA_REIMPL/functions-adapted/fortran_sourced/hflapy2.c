/*
 * Adaptado de LAPACK (netlib.org/lapack) para media precisión
 * 
 * Copyright original:
 *   Copyright (c) 1992-2025 The University of Tennessee and The University
 *                        of Tennessee Research Foundation. All rights reserved.
 *   Copyright (c) 2000-2025 The University of California Berkeley. All rights reserved.
 *   Copyright (c) 2006-2025 The University of Colorado Denver. All rights reserved.
 * 
 * Modificaciones (c) 2025 Eloi Barcón Piñeiro
 * 
 * Licencia: BSD modificada (ver ../../../../LICENSE_LAPACK)
 */

#include "lapacke_utils_reimpl.h"

/**
 * \file hflapy2.c
 * \brief Cálculo seguro de la norma euclídea en lapack_float
 */

/**
 * \brief Calcula sqrt(x² + y²) evitando desbordamientos en lapack_float
 * 
 * \details Versión lapack_float de LAPACK's SLAPY2. Implementa:
 *          - Manejo de NaN: Si x o y es NaN, retorna inmediatamente NaN
 *          - Evita overflow mediante escalado: sqrt(w²*(1 + (z/w)²)) donde w = max(|x|, |y|)
 *          - Precisión mejorada para valores pequeños
 * 
 * \param[in] x Primer valor lapack_float
 * \param[in] y Segundo valor lapack_float
 * \return lapack_float Resultado de sqrt(x² + y²)
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
 * - Requiere que custom_sqrtf_half_precision maneje entradas en rango [1, 2]
 * - No verifica overflow en custom_sqrtf_half_precision (asume w <= FP16_MAX)
 * 
 * \par Algoritmo:
 * 1. Detección temprana de NaN
 * 2. Cálculo de valores absolutos
 * 3. Escalado con w = max(|x|, |y|)
 * 4. Cálculo seguro de 1 + (z/w)²
 * 
 * \example
 * lapack_float norm = hflapy2(3.0F16, 4.0F16); // 5.0F16
 * lapack_float nan_res = hflapy2(NAN_Float16, 1.0F16); // NAN_Float16
 * 
 * \see custom_sqrtf_half_precision Para implementación de raíz cuadrada en _Float16
 * \see LAPACK_HFISNAN Macro para detección de NaN en lapack_float
 */

lapack_float hflapy2(lapack_float x, lapack_float y) {
    if (LAPACK_HFISNAN(x)) {
        return x;
    }
    if (LAPACK_HFISNAN(y)) {
        return y;
    }

    lapack_float xabs = ABS_half_precision(x);
    lapack_float yabs = ABS_half_precision(y);
    lapack_float w = MAX(xabs, yabs);
    lapack_float z = MIN(xabs, yabs);

    #ifndef USE_BF16
    if (z == (lapack_float)0.0 || w > (lapack_float)FP16_MAX) {
    #else
    if (z == (lapack_float)0.0 || w > (lapack_float)BF16_MAX) {
    #endif
    
        return w;
    } else {
        lapack_float ratio = z / w;
        return w * custom_sqrtf_half_precision((lapack_float)1.0 + ratio * ratio);
    }
}