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
 * \file hflamch_half_precision.c
 * \brief Implementación de la función hflamch para obtener constantes numéricas en precisión media (_Float16, __fp16 y __bf16).
 */

/**
 * \brief Obtiene parámetros numéricos de la aritmética lapack_float
 * 
 * \details Función equivalente a LAPACK's SLAMCH para lapack_float. Retorna constantes clave 
 *          según el estándar IEEE 754-2008 binary16 con ajustes específicos para LAPACK.
 * 
 * \param[in] cmach Carácter que especifica la constante a obtener (case-insensitive):
 *                 - 'E': Épsilon de máquina (\f$2^{-10}\f$)
 *                 - 'S': Mínimo seguro (\f$\max(1/\text{MAX}, \text{TRUE\_MIN}) \times (1 + \epsilon)\f$)
 *                 - 'U': Mínimo subnormal positivo (\f$2^{-24}\f$)
 *                 - 'O': Máximo valor representable (\f$65504.0\f$)
 *                 - 'B': Base del sistema (\f$2\f$)
 *                 - 'P': Precisión (\f$\epsilon \times \text{base}\f$)
 *                 - 'N': Dígitos significativos (\f$11\f$ bits de mantisa)
 *                 - 'R': Modo redondeo (\f$1.0\f$ = redondeo al más cercano)
 *                 - 'M': Exponente mínimo (\f$-14\f$)
 *                 - 'L': Exponente máximo (\f$15\f$)
 * 
 * \return lapack_float Valor de la constante solicitada o 0.0F16 para opción inválida
 * 
 * \note
 * - El cálculo para 'S' evita underflow usando FP16_MAX: \f$\text{small} = 1/\text{huge\_val}\f$
 * - 'S' garantiza: \f$\text{sfmin} = \begin{cases} \text{small} \times (1+\epsilon) & \text{si } \text{small} \geq \text{tiny} \\ \text{tiny} & \text{en otro caso} \end{cases}\f$
 * - Compatible con ARM __fp16, __bf16 y _Float16 (GCC y compiladores modernos)
 * 
 * \warning
 * - No valida cadenas vacías (comportamiento indefinido si cmach == NULL)
 * - Caracteres adicionales en cmach son ignorados
 * - El valor 0.0F16 se retorna para opciones no implementadas
 * 
 * \example
 * lapack_float eps = hflamch_half_precision('E');       // 0.0009765625 (2^-10)
 * lapack_float safe_min = hflamch_half_precision('s');  // ≈1.5273e-05 (depende de FP16_MAX)
 * lapack_float invalid = hflamch_half_precision('X');   // 0.0F16
 * 
 * \ingroup IEEE754_FP16
 * \see FP16_SAFE_MIN  Para detalles del cálculo del mínimo seguro
 */

lapack_float hflamch_half_precision(const char cmach) {
    const char c = toupper(cmach);
    #ifndef USE_BF16
    switch(c) {
        case 'E': return (lapack_float)FP16_EPSILON;
        case 'S': 
            {  // Cálculo seguro al estilo LAPACK
                const lapack_float tiny = (lapack_float) FP16_TRUE_MIN;
                const lapack_float huge_val = (lapack_float) FP16_MAX;
                const lapack_float small = (lapack_float) 1.0 / huge_val;
                return (small >= tiny) ? small * ((lapack_float)1.0 + (lapack_float)FP16_EPSILON) : tiny;
            }
        case 'U': return (lapack_float)FP16_TRUE_MIN;
        case 'O': return (lapack_float)FP16_MAX;
        case 'B': return (lapack_float)FP16_RADIX;
        case 'P': return (lapack_float)FP16_EPSILON * (lapack_float)FP16_RADIX;
        case 'N': return (lapack_float)FP16_MANT_DIG;
        case 'R': return (lapack_float)1.0;  // Round to nearest (TIE_EVEN)
        case 'M': return (lapack_float)FP16_MIN_EXP;
        case 'L': return (lapack_float)FP16_MAX_EXP;
        default:  return (lapack_float)0.0;
    }
    #else
        switch(c) {
        case 'E': return (lapack_float)BF16_EPSILON;
        case 'S': 
            {  // Cálculo seguro al estilo LAPACK
                const lapack_float tiny = (lapack_float) BF16_TRUE_MIN;
                const lapack_float huge_val = (lapack_float) BF16_MAX;
                const lapack_float small = (lapack_float) 1.0 / huge_val;
                return (small >= tiny) ? small * ((lapack_float)1.0 + (lapack_float)BF16_EPSILON) : tiny;
            }
        case 'U': return (lapack_float)BF16_TRUE_MIN;
        case 'O': return (lapack_float)BF16_MAX;
        case 'B': return (lapack_float)BF16_RADIX;
        case 'P': return (lapack_float)BF16_EPSILON * (lapack_float)BF16_RADIX;
        case 'N': return (lapack_float)BF16_MANT_DIG;
        case 'R': return (lapack_float)1.0;  // Round to nearest (TIE_EVEN)
        case 'M': return (lapack_float)BF16_MIN_EXP;
        case 'L': return (lapack_float)BF16_MAX_EXP;
        default:  return (lapack_float)0.0;
    }
    #endif
}
