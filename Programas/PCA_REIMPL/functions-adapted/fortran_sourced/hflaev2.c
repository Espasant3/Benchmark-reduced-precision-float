
#include "lapacke_utils_reimpl.h"

/**
 * \file hflaev2.c
 * \brief Cálculo de autovalores y autovectores para matrices simétricas 2x2 en lapack_float
 */

/**
 * \brief Calcula autovalores y autovectores de una matriz simétrica 2x2 en lapack_float
 * 
 * \details Versión lapack_float de LAPACK's SLAEV2. Resuelve:
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
 * - Precisión limitada en lapack_float puede afectar ortogonalidad
 * - Requiere custom_sqrtf_half_precision para raíz cuadrada en _Float16
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
 * lapack_float rt1, rt2, cs1, sn1;
 * hflaev2(2.0F16, 1.0F16, 3.0F16, &rt1, &rt2, &cs1, &sn1);
 * // rt1 ≈ 4.0F16, rt2 ≈ 1.0F16
 * // cs1 ≈ 0.707F16, sn1 ≈ 0.707F16
 * 
 * \see LAPACK SLAEV2 Para versión en precisión simple
 * \see custom_sqrtf_half_precision Implementación de raíz cuadrada en _Float16
 * \see ABS_half_precision Macro para valor absoluto en lapack_float
 */

void hflaev2(lapack_float a, lapack_float b, lapack_float c, lapack_float *rt1, lapack_float *rt2, lapack_float *cs1, lapack_float *sn1) {
    
    // Constantes
    const lapack_float ONE = (lapack_float)1.0;
    const lapack_float TWO = (lapack_float)2.0;
    const lapack_float HALF = (lapack_float)0.5;
    
    lapack_float sm = a + c;
    lapack_float df = a - c;
    lapack_float adf = ABS_half_precision(df);
    lapack_float tb = TWO * b;
    lapack_float ab = ABS_half_precision(tb);
    lapack_float acmx, acmn;

    // Determine the larger and smaller of |A| and |C|
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

    int sgn1, sgn2;

    // Compute eigenvalues
    if (sm < (lapack_float)0.0) {
        *rt1 = HALF * (sm - rt);
        sgn1 = -1;
        lapack_float rt1_val = *rt1;
        *rt2 = (acmx / rt1_val) * acmn - (b / rt1_val) * b;
    } else if (sm > (lapack_float)0.0) {
        *rt1 = HALF * (sm + rt);
        sgn1 = 1;
        lapack_float rt1_val = *rt1;
        *rt2 = (acmx / rt1_val) * acmn - (b / rt1_val) * b;
    } else {
        // Handle case where sm is zero
        *rt1 = HALF * rt;
        *rt2 = -HALF * rt;
        sgn1 = 1;
    }

    // Compute eigenvector
    lapack_float cs;
    if (df >= (lapack_float)0.0) {
        cs = df + rt;
        sgn2 = 1;
    } else {
        cs = df - rt;
        sgn2 = -1;
    }

    lapack_float acs = ABS_half_precision(cs);
    if (acs > ab) {
        lapack_float ct = -tb / cs;
        *sn1 = ONE / custom_sqrtf_half_precision(ONE + ct * ct);
        *cs1 = ct * (*sn1);
    } else {
        if (ab == (lapack_float)0.0) {
            *cs1 = ONE;
            *sn1 = (lapack_float)0.0;
        } else {
            lapack_float tn = -cs / tb;
            *cs1 = ONE / custom_sqrtf_half_precision(ONE + tn * tn);
            *sn1 = tn * (*cs1);
        }
    }

    // Adjust eigenvector signs if necessary
    if (sgn1 == sgn2) {
        lapack_float temp = *cs1;
        *cs1 = -(*sn1);
        *sn1 = temp;
    }
}