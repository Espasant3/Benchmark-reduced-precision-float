
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file ieeeck_reimpl.c
 * \brief Implementación de la verificación de cumplimiento IEEE 754 en C
 */

/**
 * \brief Comprueba el cumplimiento del estándar IEEE 754 en el entorno de ejecución
 * 
 * \details Implementación inspirada en ieeeck de LAPACK que realiza 12 pruebas críticas:
 *          1. Existencia de infinitos positivos/negativos
 *          2. Comportamiento correcto del cero con signo
 *          3. Propagación correcta de NaN
 *          4. Aritmética especial según sección 7 de IEEE 754-2008
 * 
 * \param[in] ispec Tipo de verificación:
 *                 - 0: Solo verificación de infinitos y cero con signo
 *                 - Cualquier otro valor: Verificación completa (infinitos + NaNs)
 * \param[in] zero Puntero al valor 0.0 en el formato a verificar
 * \param[in] one Puntero al valor 1.0 en el formato a verificar
 * 
 * \return int Resultado de la verificación:
 *            - 1: Cumple con los requisitos IEEE 754 verificados
 *            - 0: No cumple con el estándar o se detectó comportamiento no IEEE
 * 
 * \note
 * - Usa variables volatile para evitar optimizaciones que alteren los resultados
 * - Incluye 6 pruebas diferentes de generación/propagación de NaN
 * - Requiere que FPU esté en modo estricto (sin flush-to-zero)
 * 
 * \warning
 * - Depende del comportamiento del compilador con operaciones de división por cero
 * - No thread-safe debido al uso de variables static internas en algunas implementaciones
 * - Los resultados pueden variar con opciones de compilación -ffast-math
 * 
 * \par Ejemplo de uso:
 * \code
 * float zero = 0.0f, one = 1.0f;
 * int ieee_ok = ieeeck_reimpl(1, &zero, &one);  // Verificación completa
 * \endcode
 * 
 * \par Pruebas realizadas:
 * 1. Existencia de +inf y -inf
 * 2. Cálculo correcto de -0.0
 * 3. Operaciones con valores especiales:
 *    - inf + (-inf) → NaN
 *    - inf / -inf → NaN
 *    - inf / inf → NaN
 *    - inf * 0.0 → NaN
 * 
 * \see IEEE 754-2008 Standard: Sección 7 (Operaciones con valores especiales)
 * \see ilaenv_reimpl Para selección de algoritmos dependientes de IEEE
 */

int ieeeck_reimpl(int ispec, float *zero, float *one) {
    volatile float zero_val = *zero;  // Evita optimizaciones
    volatile float one_val = *one;
    volatile float posinf, neginf, negzro, newzro;
    volatile float nan1, nan2, nan3, nan4, nan5, nan6;

    // Verificar infinitos
    posinf = one_val / zero_val;
    if (posinf <= one_val) return 0;

    neginf = -one_val / zero_val;
    if (neginf >= zero_val) return 0;

    negzro = one_val / (neginf + one_val);
    if (negzro != zero_val) return 0;

    neginf = one_val / negzro;
    if (neginf >= zero_val) return 0;

    newzro = negzro + zero_val;
    if (newzro != zero_val) return 0;

    posinf = one_val / newzro;
    if (posinf <= one_val) return 0;

    neginf = neginf * posinf;
    if (neginf >= zero_val) return 0;

    posinf = posinf * posinf;
    if (posinf <= one_val) return 0;

    if (ispec == 0) return 1;

    // Verificar NaNs
    nan1 = posinf + neginf;
    if (nan1 == nan1) return 0;

    nan2 = posinf / neginf;
    if (nan2 == nan2) return 0;

    nan3 = posinf / posinf;
    if (nan3 == nan3) return 0;

    nan4 = posinf * zero_val;
    if (nan4 == nan4) return 0;

    nan5 = neginf * negzro;
    if (nan5 == nan5) return 0;

    nan6 = nan5 * zero_val;
    if (nan6 == nan6) return 0;

    return 1;
}