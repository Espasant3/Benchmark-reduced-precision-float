
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file hflae2.c
 * \brief Cálculo de autovalores para matrices simétricas 2x2 en _Float16
 */

/**
 * \brief Calcula los autovalores de una matriz simétrica 2x2 en _Float16
 * 
 * \details Versión _Float16 de LAPACK's SLAE2. Calcula los autovalores de:
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
 *   Parámetros modificados para usar _Float16
 *   \endcode
 * - Orden de autovalores: rt1 + rt2 = a + c, rt1 * rt2 = ac - b²
 * - Uso interno en rutinas de descomposición tridiagonal
 * 
 * \warning
 * - Precisión limitada de _Float16 puede afectar resultados vs versión double
 * - No manejo especial de overflow/underflow (depende de hardware)
 * - Requiere implementación de custom_sqrtf16 y ABS_Float16
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
 * _Float16 rt1, rt2;
 * hflae2(3.0F16, 1.0F16, 2.0F16, &rt1, &rt2);
 * // rt1 ≈ 3.618F16, rt2 ≈ 1.382F16
 * 
 * \see LAPACK SLAE2 Para versión en precisión simple
 * \see custom_sqrtf16 Implementación de raíz cuadrada en _Float16
 * \see ABS_Float16 Valor absoluto optimizado para _Float16
 */

void hflae2(_Float16 a, _Float16 b, _Float16 c, _Float16 *rt1, _Float16 *rt2) {
    _Float16 sm = a + c;
    _Float16 df = a - c;
    _Float16 adf = ABS_Float16(df);
    _Float16 tb = 2.0F16 * b;
    _Float16 ab = ABS_Float16(tb);
    _Float16 acmx, acmn;

    if (ABS_Float16(a) > ABS_Float16(c)) {
        acmx = a;
        acmn = c;
    } else {
        acmx = c;
        acmn = a;
    }

    _Float16 rt;
    if (adf > ab) {
        rt = adf * custom_sqrtf16(1.0F16 + (ab / adf) * (ab / adf));
    } else if (adf < ab) {
        rt = ab * custom_sqrtf16(1.0F16 + (adf / ab) * (adf / ab));
    } else {
        rt = ab * custom_sqrtf16(2.0F16);
    }

    if (sm < 0.0F16) {
        *rt1 = 0.5F16 * (sm - rt);
        *rt2 = (acmx / *rt1) * acmn - (b / *rt1) * b;
    } else if (sm > 0.0F16) {
        *rt1 = 0.5F16 * (sm + rt);
        *rt2 = (acmx / *rt1) * acmn - (b / *rt1) * b;
    } else {
        *rt1 = 0.5F16 * rt;
        *rt2 = -(*rt1);
    }
}