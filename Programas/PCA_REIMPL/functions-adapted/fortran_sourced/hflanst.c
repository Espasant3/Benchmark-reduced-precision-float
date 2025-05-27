
#include "lapacke_utils_reimpl.h"

/**
 * \file hflanst.c
*/

/**
 * \brief Calcula la norma de una matriz tridiagonal simétrica en lapack_float
 * 
 * \details Versión lapack_float de LAPACK's slanst. Soporta múltiples tipos de norma:
 *          - Máximo absoluto ('M')
 *          - Norma 1/Infinito ('1', 'O', 'I')
 *          - Norma Frobenius/Euclídea ('F', 'E')
 * 
 * \param[in] norm Tipo de norma a calcular (case-insensitive):
 *                - 'M': Valor máximo absoluto |A(i,j)|
 *                - '1', 'O': Norma 1 (suma máxima por columnas)
 *                - 'I': Norma Infinito (suma máxima por filas)
 *                - 'F', 'E': Norma Frobenius (sqrt(Σ|A(i,j)|²))
 * \param[in] n    Orden de la matriz (n >= 0)
 * \param[in] d    Array lapack_float[n] con elementos diagonales
 * \param[in] e    Array lapack_float[n-1] con elementos sub/superdiagonales
 * 
 * \return lapack_float Valor de la norma solicitada
 * 
 * \note
 * - Para n=0 retorna 0 inmediatamente
 * - En normas 1/Infinito, para matrices tridiagonales simétricas ambas son iguales
 * - La norma Frobenius usa escalado para evitar overflow (algoritmo de Blue)
 * 
 * \warning
 * - No valida punteros d/e (comportamiento indefinido si son NULL)
 * - No verifica consistencia entre n y tamaño de d/e
 * - El cálculo de Frobenius depende de custom_sqrtf_half_precision
 * 
 * \par Algoritmo por tipo de norma:
 * 1. 'M': Máximo de |d[i]| y |e[i]| ∀i
 * 2. '1'/'O'/'I': Suma máxima de |d[i]| + |e[i-1]| + |e[i]|
 * 3. 'F'/'E': sqrt(Σ|d[i]|² + 2Σ|e[i]|²) con escalado numérico
 * 
 * \example
 * lapack_float d[3] = {1.5F16, 2.3F16, 0.7F16};
 * lapack_float e[2] = {0.5F16, -1.2F16};
 * 
 * lapack_float max_norm = hflanst('M', 3, d, e);        // 2.3
 * lapack_float frob_norm = hflanst('F', 3, d, e);       // sqrt(1.5² + 2*(0.5² + 1.2²) + 2.3² + 0.7²)
 * 
 * \see lsame_reimpl Para comparación case-insensitive de caracteres
 * \see custom_sqrtf_half_precision Implementación de raíz cuadrada en _Float16
 * \see LAPACK_HFISNAN Macro para detección de NaN en lapack_float
 */

lapack_float hflanst(char norm, int n, const lapack_float *d, const lapack_float *e) {
    
    // Constantes
    const lapack_float ZERO = (lapack_float)0.0;
    const lapack_float ONE = (lapack_float)1.0;
    const lapack_float TWO = (lapack_float)2.0;

    
    lapack_float anorm = ZERO;
    if (n <= 0) return anorm;

    if (lsame_reimpl(norm, 'M')) {
        /* Norma Máximo */
        anorm = ABS_half_precision(d[n-1]);
        for (int i = 0; i < n-1; ++i) {
            lapack_float current = ABS_half_precision(d[i]);
            if (current > anorm || LAPACK_HFISNAN(current)) anorm = current;
            
            current = ABS_half_precision(e[i]);
            if (current > anorm || LAPACK_HFISNAN(current)) anorm = current;
        }
    } 
    else if (lsame_reimpl(norm, '1') || lsame_reimpl(norm, 'O') || lsame_reimpl(norm, 'I')) {
        /* Normas 1/Infinito */
        if (n == 1) {
            anorm = ABS_half_precision(d[0]);
        } else {
            anorm = ABS_half_precision(d[0]) + ABS_half_precision(e[0]);
            lapack_float sum = ABS_half_precision(e[n-2]) + ABS_half_precision(d[n-1]);
            if (sum > anorm || LAPACK_HFISNAN(sum)) anorm = sum;
            
            for (int i = 1; i < n-1; ++i) {
                sum = ABS_half_precision(d[i]) + ABS_half_precision(e[i]) + ABS_half_precision(e[i-1]);
                if (sum > anorm || LAPACK_HFISNAN(sum)) anorm = sum;
            }
        }
    } 
    else if (lsame_reimpl(norm, 'F') || lsame_reimpl(norm, 'E')) {
        /* Norma Frobenius */
        lapack_float scale = ZERO;
        lapack_float sum = ONE;
        
        if (n > 1) {
            for (int i = 0; i < n-1; ++i) {
                lapack_float absxi = ABS_half_precision(e[i]);
                if (absxi != ZERO) {
                    if (scale < absxi) {
                        sum = ONE + sum * (scale/absxi) * (scale/absxi);
                        scale = absxi;
                    } else {
                        sum += (absxi/scale) * (absxi/scale);
                    }
                }
            }
            sum *= TWO;
        }
        
        for (int i = 0; i < n; ++i) {
            lapack_float absxi = ABS_half_precision(d[i]);
            if (absxi != ZERO) {
                if (scale < absxi) {
                    sum = ONE + sum * (scale/absxi) * (scale/absxi);
                    scale = absxi;
                } else {
                    sum += (absxi/scale) * (absxi/scale);
                }
            }
        }
        
        anorm = scale * custom_sqrtf_half_precision(sum);
    }
    
    return anorm;
}