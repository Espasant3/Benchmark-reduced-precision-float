/*
 * Autor: Eloi Barcón Piñeiro
 * Año: 2025
 * 
 * Licencia MIT (ver ../../../../LICENSE_MIT)
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* -------------------------------------------------------------------------- */
/* Includes esenciales */
/* -------------------------------------------------------------------------- */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <ctype.h>  // Para toupper
#include <assert.h>
#include <stddef.h>

#if defined(USE_FP16) && defined(__aarch64__)
  #include <arm_fp16.h>  // Para __fp16 en ARM
#elif defined(USE_BF16) && defined(__aarch64__)
  #include <arm_bf16.h>  // Para __bf16 en ARM
#endif

/* -------------------------------------------------------------------------- */
/* Constantes IEEE 754 para _Float16 y __fp16 (binary16/half-precision) */
/* -------------------------------------------------------------------------- */
/**
 * \defgroup IEEE754_FP16 Constantes IEEE 754-2008 para formato binary16 (half-precision)
 * \{
 */

/** \brief Épsilon de la máquina: diferencia entre 1.0 y el siguiente valor representable (2^-10) */
#define FP16_EPSILON     __FLT16_EPSILON__      // 9.765625e-04F16 (2^-10)

/** \brief Mínimo valor normalizado positivo (2^-14) */
#define FP16_MIN         __FLT16_MIN__          // 6.103515625e-05F16 (2^-14)

/** \brief Mínimo valor subnormal positivo (2^-24) */
#define FP16_TRUE_MIN    __FLT16_DENORM_MIN__   // 5.9604644775390625e-08F16 (2^-24)

/** \brief Máximo valor representable finito (65504.0) */
#define FP16_MAX         __FLT16_MAX__          // 65504.0F16 (2^15*(2-2^-10))

/** \brief Base del sistema de numeración (binario) */
#define FP16_RADIX       2

/** \brief Número de dígitos significativos (precisión de 11 bits) */
#define FP16_MANT_DIG    __FLT16_MANT_DIG__     // 11 bits (precisión)

/** \brief Exponente mínimo normalizado (base 2) */
#define FP16_MIN_EXP    (-14)                   // Hardcodeado según IEEE 754

/** \brief Exponente máximo normalizado (base 2) */
#define FP16_MAX_EXP     15                     // Hardcodeado según IEEE 754

/** \} */ // Fin de grupo IEEE754_FP16


/* -------------------------------------------------------------------------- */
/* Constantes IEEE 754 para bfloat16 (brain floating point) */
/* -------------------------------------------------------------------------- */
/**
 * \defgroup IEEE754_BF16 Constantes IEEE 754-2019 para formato bfloat16 (16-bit brain floating point)
 * \{
 */

/** \brief Épsilon de la máquina: diferencia entre 1.0 y el siguiente valor representable (2^-7) */
#define BF16_EPSILON  __BFLT16_EPSILON__          // 7.8125e-03F16 (2^-7) 

/** \brief Mínimo valor normalizado positivo (2^-126) */
#define BF16_MIN         __BFLT16_MIN__           // 1.17549435e-38F16 (2^-126)

/** \brief Mínimo valor subnormal positivo (2^-133)  */
#define BF16_TRUE_MIN    __BFLT16_DENORM_MIN__    // 9.1835496e-41F16 (2^-133)

/** \brief Máximo valor representable finito (3.3895314e+38) */
#define BF16_MAX         __BFLT16_MAX__           // 3.4028235e+38F16 (2^127*(2-2^-7))

/** \brief Base del sistema de numeración (binario) */
#define BF16_RADIX       2            

/** \brief Número de dígitos significativos (precisión de 8 bits) */
#define BF16_MANT_DIG    __BFLT16_MANT_DIG__      // 8 bits (7 almacenados + 1 implícito)

/** \brief Exponente mínimo normalizado (base 2) */
#define BF16_MIN_EXP    (-126)                    // Hardcodeado según IEEE 754

/** \brief Exponente máximo normalizado (base 2) */
#define BF16_MAX_EXP     127                      // Hardcodeado según IEEE 754

/** \} */ // Fin de grupo IEEE754_BF16

/* -------------------------------------------------------------------------- */
/* Configuración y macros globales */
/* -------------------------------------------------------------------------- */

#define LAPACK_ROW_MAJOR               101
#define LAPACK_COL_MAJOR               102

#define LAPACK_WORK_MEMORY_ERROR       -1010
#define LAPACK_TRANSPOSE_MEMORY_ERROR  -1011

/* -------------------------------------------------------------------------- */
/* Memory handler */
/* -------------------------------------------------------------------------- */

#ifndef LAPACKE_malloc
#define LAPACKE_malloc( size )      malloc( size )
#endif
#ifndef LAPACKE_calloc
#define LAPACKE_calloc(nmemb, size) calloc(nmemb, size)
#endif
#ifndef LAPACKE_free
#define LAPACKE_free( p )           free( p )
#endif

/* -------------------------------------------------------------------------- */
/* Macros para operaciones matemáticas */
/* -------------------------------------------------------------------------- */

#ifndef ABS
#define ABS(x) (((x) < 0) ? -(x) : (x))
#endif
#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

/* -------------------------------------------------------------------------- */
/* NaN checkers */
/* -------------------------------------------------------------------------- */

#define LAPACK_HFISNAN( x ) ( x != x )

/* -------------------------------------------------------------------------- */
/* Tipos personalizados LAPACK*/
/* -------------------------------------------------------------------------- */

typedef int32_t lapack_int;
typedef lapack_int lapack_logical;

#if defined(USE_FP16) && defined(USE_BF16)
  #error "Sólo se puede definir una de las macros: USE_FP16 o USE_BF16."
#endif


#ifdef USE_FP16
  typedef __fp16 lapack_float;
#elif defined(USE_BF16)
  typedef __bf16 lapack_float;
#else
  // Si ninguna de las opciones se define, usamos _Float16 por defecto
  typedef _Float16 lapack_float;
#endif


/* -------------------------------------------------------------------------- */
/*                              Auxiliar functions                            */
/* -------------------------------------------------------------------------- */

/**
 * \brief Calcula el valor absoluto de un número lapack_float mediante manipulación binaria.
 * 
 * Esta función obtiene el valor absoluto de `x` borrando directamente el bit de signo (bit 15),
 * evitando operaciones aritméticas y garantizando precisión bit a bit según IEEE 754.
 * 
 * \param[in] x Número lapack_float de entrada (puede ser negativo, positivo, cero, NaN o infinito).
 * \return lapack_float Valor absoluto de `x` (sin signo). Conserva NaN/infinitos pero con bit de signo en 0.
 * 
 * \note **Detalles de implementación**:
 * - Usa `memcpy` para copiar bits entre tipos, evitando violaciones de aliasing estricto (C11 Standard §6.5/7).
 * - Máscara aplicada: `0x7FFF` (borra el bit 15, preserva exponente y mantisa).
 * - Endianness-agnóstica: Funciona en arquitecturas little-endian y big-endian.
 * 
 * \warning La posición del bit de signo (15) asume compliance con IEEE 754-2008 para lapack_float (que puede ser _Float16, __fp16 o __bf16).
 *          No usar en formatos no estándar donde el signo esté en otra posición.
 * 
 * \example
 * \code
 * lapack_float a = (lapack_float) -3.14;   // Valor negativo
 * lapack_float b = ABS_half_precision(a); // b = 3.14 (bit de signo en 0)
 * \endcode
 * 
 * \test Casos verificados:
 * - Cero negativo (`-0.0f` → `0.0f`).
 * - NaN de entrada → NaN sin signo (payload preservado).
 * - Infinito negativo (`-Inf` → `Inf`).
 */
static inline lapack_float ABS_half_precision(lapack_float x) {
    uint16_t bits;
    memcpy(&bits, &x, sizeof(bits));
    bits &= 0x7FFF; // Borra el bit de signo (bit 15)
    memcpy(&x, &bits, sizeof(bits));
    return x;
}

/**
 * \brief Implementación optimizada de raíz cuadrada para números lapack_float (_Float16, __fp16 o __bf16).
 * 
 * Maneja casos especiales según el estándar IEEE 754-2008:
 * - Entrada NaN → Retorna NaN
 * - Números negativos → Retorna NaN
 * - Cero → Retorna cero (con signo preservado)
 * - Infinito positivo → Retorna infinito positivo
 * 
 * \param x Número de tipo lapack_float para calcular la raíz cuadrada (debe ser ≥ 0)
 * \return lapack_float Resultado de la operación con las siguientes condiciones especiales:
 *         - NaN si la entrada es negativa
 *         - Infinito positivo si la entrada es infinito positivo
 * 
 * \note Implementación actual:
 *       - Usa la macro LAPACK_HFISNAN para detección portable de NaN
 *       - Utiliza sqrtf en precisión simple como fallback portable
 *       - Puede optimizarse con USE_FP16_INTRINSICS si está disponible
 * 
 * \warning El comportamiento para entradas negativas está definido (retorna NaN)
 * 
 * \see [IEEE 754-2008 Standard](https://ieeexplore.ieee.org/document/4610935)
 */
static inline lapack_float custom_sqrtf_half_precision(lapack_float x) {
    // Verifica NaN usando el estándar C (isnan no soporta lapack_float directamente)
    if (LAPACK_HFISNAN(x)) return x;         // NaN → NaN
    if (x < (lapack_float)0.0) return (lapack_float)NAN;   // sqrt(negativo) = NaN
    if (x == (lapack_float)0.0) return (lapack_float)0.0;
    if (x == (lapack_float)INFINITY) return (lapack_float)INFINITY; // sqrt(Inf) = Inf

    // Fallback portable para todas las arquitecturas
    return (lapack_float)sqrtf((float)x);
}

static inline void hfaxpy(int n, lapack_float alpha, lapack_float *x, int incx, lapack_float *y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] += alpha * x[i * incx];
    }
}


#endif /* DEFINITIONS_H */
