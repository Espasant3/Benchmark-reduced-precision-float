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


/* -------------------------------------------------------------------------- */
/* Constantes IEEE 754 para _Float16 (binary16/half-precision) */
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


/* Opcional: control de características */
#define ENABLE_NAN_CHECK 1

/* -------------------------------------------------------------------------- */
/* Auxiliar functions */
/* -------------------------------------------------------------------------- */

/**
 * \brief Calcula el valor absoluto de un número _Float16 a nivel de bits
 * 
 * Esta función evita problemas de precisión en operaciones con coma flotante manipulando
 * directamente la representación binaria del número según el estándar IEEE 754.
 * 
 * \param x Número de tipo _Float16 del que se obtendrá el valor absoluto
 * \return _Float16 Valor absoluto del parámetro sin signo
 * 
 * \note La función opera directamente sobre los bits del número:
 *       - Utiliza memcpy para evitar problemas de aliasing estricto
 *       - Elimina el bit de signo (bit 15, el más significativo en 16 bits)
 *       - No modifica los bits de mantisa o exponente
 */
static inline _Float16 ABS_Float16(_Float16 x) {
    uint16_t bits;
    memcpy(&bits, &x, sizeof(bits));
    bits &= 0x7FFF; // Borra el bit de signo (bit 15)
    memcpy(&x, &bits, sizeof(bits));
    return x;
}

/**
 * \brief Implementación optimizada de raíz cuadrada para números _Float16
 * 
 * Maneja casos especiales según el estándar IEEE 754-2008:
 * - Entrada NaN → Retorna NaN
 * - Números negativos → Retorna NaN
 * - Cero → Retorna cero (con signo preservado)
 * - Infinito positivo → Retorna infinito positivo
 * 
 * \param x Número de tipo _Float16 para calcular la raíz cuadrada (debe ser ≥ 0)
 * \return _Float16 Resultado de la operación con las siguientes condiciones especiales:
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
static inline _Float16 custom_sqrtf16(_Float16 x) {
    // Verifica NaN usando el estándar C (isnan no soporta _Float16 directamente)
    if (LAPACK_HFISNAN(x)) return x;         // NaN → NaN
    if (x < 0.0F16) return NAN;   // sqrt(negativo) = NaN
    if (x == 0.0F16) return 0.0F16;
    if (x == INFINITY) return INFINITY; // sqrt(Inf) = Inf

    // Fallback portable para todas las arquitecturas
    return (_Float16)sqrtf((float)x);
}

static inline void hfaxpy(int n, _Float16 alpha, _Float16 *x, int incx, _Float16 *y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] += alpha * x[i * incx];
    }
}


#endif /* DEFINITIONS_H */
