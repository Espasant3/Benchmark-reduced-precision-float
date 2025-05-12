
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file hflaev2.c
 * \brief Calcula autovalores y autovectores para matrices simétricas 2x2 en _Float16
 */

/**
 * \brief Calcula autovalores y autovectores de una matriz simétrica 2x2 en _Float16
 * 
 * \details Versión _Float16 de LAPACK's SLAEV2. Resuelve:
 *          \f[
 *          \begin{pmatrix} a & b \\ b & c \end{pmatrix} \cdot \begin{pmatrix} cs1 \\ sn1 \end{pmatrix} = rt1 \cdot \begin{pmatrix} cs1 \\ sn1 \end{pmatrix}
 *          \f]
 *          y similar para rt2. Implementa:
 *          - Cálculo numéricamente estable de autovalores
 *          - Determinación precisa del autovector principal
 *          - Manejo de casos especiales (matriz diagonal, autovalores iguales)
 * 
 * \param[in] a    Elemento (1,1) de la matriz
 * \param[in] b    Elemento (1,2) y (2,1) de la matriz
 * \param[in] c    Elemento (2,2) de la matriz
 * \param[out] rt1 Primer autovalor (mayor en valor absoluto si a ≠ c)
 * \param[out] rt2 Segundo autovalor
 * \param[out] cs1 Coseno del ángulo de rotación del autovector
 * \param[out] sn1 Seno del ángulo de rotación del autovector
 * 
 * \note
 * - Orden de autovalores: rt1 + rt2 = a + c, rt1 * rt2 = ac - b²
 * - El autovector corresponde a rt1 (columna [cs1; sn1])
 * - Para matrices diagonales (b=0): cs1=1, sn1=0
 * 
 * \warning
 * - No maneja entradas NaN (comportamiento indefinido)
 * - Precisión limitada en _Float16 puede afectar ortogonalidad
 * - Requiere custom_sqrtf16 para raíz cuadrada en _Float16
 * 
 * \par Algoritmo clave:
 * 1. Cálculo de términos intermedios:
 *    \f[
 *    sm = a + c,\quad df = a - c,\quad rt = \sqrt{df^2 + 4b^2}
 *    \f]
 * 2. Determinación de autovalores según signo de sm
 * 3. Cálculo del autovector mediante relaciones trigonométricas
 * 
 * \example
 * _Float16 rt1, rt2, cs1, sn1;
 * hflaev2(2.0F16, 1.0F16, 3.0F16, &rt1, &rt2, &cs1, &sn1);
 * // rt1 ≈ 4.0F16, rt2 ≈ 1.0F16
 * // cs1 ≈ 0.707F16, sn1 ≈ 0.707F16
 * 
 * \see LAPACK SLAEV2 Para versión en precisión simple
 * \see custom_sqrtf16 Implementación de raíz cuadrada en _Float16
 * \see ABS_Float16 Macro para valor absoluto en _Float16
 */

void hflaev2(_Float16 a, _Float16 b, _Float16 c, _Float16 *rt1, _Float16 *rt2, _Float16 *cs1, _Float16 *sn1) {
    _Float16 sm = a + c;
    _Float16 df = a - c;
    _Float16 adf = ABS_Float16(df);
    _Float16 tb = 2.0F16 * b;
    _Float16 ab = ABS_Float16(tb);
    _Float16 acmx, acmn;

    // Determine the larger and smaller of |A| and |C|
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

    int sgn1, sgn2;

    // Compute eigenvalues
    if (sm < 0.0F16) {
        *rt1 = 0.5F16 * (sm - rt);
        sgn1 = -1;
        _Float16 rt1_val = *rt1;
        *rt2 = (acmx / rt1_val) * acmn - (b / rt1_val) * b;
    } else if (sm > 0.0F16) {
        *rt1 = 0.5F16 * (sm + rt);
        sgn1 = 1;
        _Float16 rt1_val = *rt1;
        *rt2 = (acmx / rt1_val) * acmn - (b / rt1_val) * b;
    } else {
        // Handle case where sm is zero
        *rt1 = 0.5F16 * rt;
        *rt2 = -0.5F16 * rt;
        sgn1 = 1;
    }

    // Compute eigenvector
    _Float16 cs;
    if (df >= 0.0F16) {
        cs = df + rt;
        sgn2 = 1;
    } else {
        cs = df - rt;
        sgn2 = -1;
    }

    _Float16 acs = ABS_Float16(cs);
    if (acs > ab) {
        _Float16 ct = -tb / cs;
        *sn1 = 1.0F16 / custom_sqrtf16(1.0F16 + ct * ct);
        *cs1 = ct * (*sn1);
    } else {
        if (ab == 0.0F16) {
            *cs1 = 1.0F16;
            *sn1 = 0.0F16;
        } else {
            _Float16 tn = -cs / tb;
            *cs1 = 1.0F16 / custom_sqrtf16(1.0F16 + tn * tn);
            *sn1 = tn * (*cs1);
        }
    }

    // Adjust eigenvector signs if necessary
    if (sgn1 == sgn2) {
        _Float16 temp = *cs1;
        *cs1 = -(*sn1);
        *sn1 = temp;
    }
}