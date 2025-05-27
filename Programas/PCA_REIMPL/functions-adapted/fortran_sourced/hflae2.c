
#include "lapacke_utils_reimpl.h"

/**
 * \file hflae2.c
 * \brief Cálculo de autovalores para matrices simétricas 2x2 en lapack_float
 */

/**
 * \brief Calcula los autovalores de una matriz simétrica 2x2 en lapack_float
 * 
 * \details Versión lapack_float de LAPACK's SLAE2. Calcula los autovalores de:
 *          \f[
 *          \begin{pmatrix} a & b \\ b & c \end{pmatrix}
 *          \f]
 *          usando método numéricamente estable adaptado para precisión media.
 * 
 * \param[in] a   Elemento (1,1) de la matriz
 * \param[in] b   Elemento (1,2) y (2,1) de la matriz
 * \param[in] c   Elemento (2,2) de la matriz
 * \param[out] rt1 Primer autovalor (mayor en valor absoluto si a != c)
 * \param[out] rt2 Segundo autovalor
 * 
 * \note
 * - Implementación adaptada de: 
 *   \code
 *   EISPACK routine (traducción a C de la versión Fortran)
 *   Parámetros modificados para usar lapack_float
 *   \endcode
 * - Orden de autovalores: rt1 + rt2 = a + c, rt1 * rt2 = ac - b²
 * - Uso interno en rutinas de descomposición tridiagonal
 * 
 * \warning
 * - Precisión limitada de lapack_float puede afectar resultados vs versión double
 * - No manejo especial de overflow/underflow (depende de hardware)
 * - Requiere implementación de custom_sqrtf_half_precision y ABS_half_precision
 * 
 * \par Algoritmo clave:
 * 1. Cálculo de términos intermedios:
 *    \f[
 *    sm = a + c,\quad df = a - c,\quad rt = \sqrt{df^2 + 4b^2}
 *    \f]
 * 2. Determinación de autovalores según signo de sm:
 *    \f[
 *    rt1 = \frac{sm \pm rt}{2},\quad rt2 = \frac{ac - b^2}{rt1}
 *    \f]
 * 
 * \example
 * lapack_float rt1, rt2;
 * hflae2(3.0F16, 1.0F16, 2.0F16, &rt1, &rt2);
 * // rt1 ≈ 3.618F16, rt2 ≈ 1.382F16
 * 
 * \see LAPACK SLAE2 Para versión en precisión simple
 * \see custom_sqrtf_half_precision Implementación de raíz cuadrada en lapack_float
 * \see ABS_half_precision Valor absoluto optimizado para lapack_float
 */

void hflae2(lapack_float a, lapack_float b, lapack_float c, lapack_float *rt1, lapack_float *rt2) {
    
    // Constantes
    const lapack_float ZERO = (lapack_float)0.0;
    const lapack_float ONE = (lapack_float)1.0;
    const lapack_float TWO = (lapack_float)2.0;
    const lapack_float HALF = (lapack_float)0.5;
    
    lapack_float sm = a + c;
    lapack_float df = a - c;
    lapack_float adf = ABS_half_precision(df);
    lapack_float tb = TWO * b;
    lapack_float ab = ABS_half_precision(tb);
    lapack_float acmx, acmn;

    if (ABS_half_precision(a) > ABS_half_precision(c)) {
        acmx = a;
        acmn = c;
    } else {
        acmx = c;
        acmn = a;
    }

    lapack_float rt;
    if (adf > ab) {
        rt = adf * custom_sqrtf_half_precision(ONE + (ab / adf) * (ab / adf));
    } else if (adf < ab) {
        rt = ab * custom_sqrtf_half_precision(ONE + (adf / ab) * (adf / ab));
    } else {
        rt = ab * custom_sqrtf_half_precision(TWO);
    }

    if (sm < ZERO) {
        *rt1 = HALF * (sm - rt);
        *rt2 = (acmx / *rt1) * acmn - (b / *rt1) * b;
    } else if (sm > ZERO) {
        *rt1 = HALF * (sm + rt);
        *rt2 = (acmx / *rt1) * acmn - (b / *rt1) * b;
    } else {
        *rt1 = HALF * rt;
        *rt2 = -(*rt1);
    }
}