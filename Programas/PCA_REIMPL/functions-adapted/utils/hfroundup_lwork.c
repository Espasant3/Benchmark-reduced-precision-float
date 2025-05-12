
#include "../include/lapacke_utils_reimpl.h" 

/**
 * \file hfroundup_lwork.c
 * \brief Implementación de la función sroundup_lwork en precisión media (_Float16)
 */

/**
 * \brief Ajusta tamaños de workspace enteros para evitar truncamiento en _Float16
 * 
 * \details Añade un ULP (Unit in Last Place) usando FP16_EPSILON cuando el entero 
 *          no es representable exactamente en _Float16.
 * 
 * \param[in] lwork Tamaño calculado en precisión superior (entero)
 * \return _Float16 Valor ajustado seguro para uso en operaciones FP16
 * 
 * \note Solo depende de FP16_EPSILON, no requiere otras constantes IEEE754_FP16
 * \warning Si lwork > 65504, el resultado es indefinido por overflow
 * 
 * \example
 * int lwork = 5000; // No representable exactamente en FP16
 * _Float16 lwork16 = hfroundup_lwork(lwork); // ≈5000.9765625
 */

_Float16 hfroundup_lwork(int lwork) {
    _Float16 result = (_Float16)lwork;
    if ((int)result < lwork) {
        result *= (1.0F16 + FP16_EPSILON);
    }
    return result;
}