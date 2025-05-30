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
 * \file hfroundup_lwork.c
 * \brief Implementación de la función sroundup_lwork en precisión media lapack_float (_Float16, __fp16 o __bf16)
 */

/**
 * \brief Ajusta tamaños de workspace enteros para evitar truncamiento en lapack_float
 * 
 * \details Añade un ULP (Unit in Last Place) usando FP16_EPSILON cuando el entero 
 *          no es representable exactamente en lapack_float.
 * 
 * \param[in] lwork Tamaño calculado en precisión superior (entero)
 * \return lapack_float Valor ajustado seguro para uso en operaciones FP16
 * 
 * \note Solo depende de FP16_EPSILON, no requiere otras constantes IEEE754_FP16
 * \warning Si lwork > 65504, el resultado es indefinido por overflow
 * 
 * \example
 * int lwork = 5000; // No representable exactamente en FP16
 * lapack_float lwork16 = hfroundup_lwork(lwork); // ≈5000.9765625
 */

lapack_float hfroundup_lwork(int lwork) {
    lapack_float result = (lapack_float)lwork;
    if ((int)result < lwork) {
        #ifndef USE_BF16
        result *= ((lapack_float)1.0 + (lapack_float) FP16_EPSILON);
        #else
        result *= ((lapack_float)1.0 + (lapack_float)BF16_EPSILON);
        #endif
    }
    return result;
}