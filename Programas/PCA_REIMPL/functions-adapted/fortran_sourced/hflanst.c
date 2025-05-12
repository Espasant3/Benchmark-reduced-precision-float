
#include "../include/lapacke_utils_reimpl.h"

/**
 * \brief Calcula la norma de una matriz tridiagonal simétrica en _Float16
 * 
 * \details Versión _Float16 de LAPACK's slanst. Soporta múltiples tipos de norma:
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
 * \param[in] d    Array _Float16[n] con elementos diagonales
 * \param[in] e    Array _Float16[n-1] con elementos sub/superdiagonales
 * 
 * \return _Float16 Valor de la norma solicitada
 * 
 * \note
 * - Para n=0 retorna 0 inmediatamente
 * - En normas 1/Infinito, para matrices tridiagonales simétricas ambas son iguales
 * - La norma Frobenius usa escalado para evitar overflow (algoritmo de Blue)
 * 
 * \warning
 * - No valida punteros d/e (comportamiento indefinido si son NULL)
 * - No verifica consistencia entre n y tamaño de d/e
 * - El cálculo de Frobenius depende de custom_sqrtf16
 * 
 * \par Algoritmo por tipo de norma:
 * 1. 'M': Máximo de |d[i]| y |e[i]| ∀i
 * 2. '1'/'O'/'I': Suma máxima de |d[i]| + |e[i-1]| + |e[i]|
 * 3. 'F'/'E': sqrt(Σ|d[i]|² + 2Σ|e[i]|²) con escalado numérico
 * 
 * \example
 * _Float16 d[3] = {1.5F16, 2.3F16, 0.7F16};
 * _Float16 e[2] = {0.5F16, -1.2F16};
 * 
 * _Float16 max_norm = hflanst('M', 3, d, e);        // 2.3
 * _Float16 frob_norm = hflanst('F', 3, d, e);       // sqrt(1.5² + 2*(0.5² + 1.2²) + 2.3² + 0.7²)
 * 
 * \see lsame_reimpl Para comparación case-insensitive de caracteres
 * \see custom_sqrtf16 Implementación de raíz cuadrada en _Float16
 * \see LAPACK_HFISNAN Macro para detección de NaN en _Float16
 */

_Float16 hflanst(char norm, int n, const _Float16 *d, const _Float16 *e) {
    _Float16 anorm = 0.0F16;
    if (n <= 0) return anorm;

    if (lsame_reimpl(norm, 'M')) {
        /* Norma Máximo */
        anorm = ABS_Float16(d[n-1]);
        for (int i = 0; i < n-1; ++i) {
            _Float16 current = ABS_Float16(d[i]);
            if (current > anorm || LAPACK_HFISNAN(current)) anorm = current;
            
            current = ABS_Float16(e[i]);
            if (current > anorm || LAPACK_HFISNAN(current)) anorm = current;
        }
    } 
    else if (lsame_reimpl(norm, '1') || lsame_reimpl(norm, 'O') || lsame_reimpl(norm, 'I')) {
        /* Normas 1/Infinito */
        if (n == 1) {
            anorm = ABS_Float16(d[0]);
        } else {
            anorm = ABS_Float16(d[0]) + ABS_Float16(e[0]);
            _Float16 sum = ABS_Float16(e[n-2]) + ABS_Float16(d[n-1]);
            if (sum > anorm || LAPACK_HFISNAN(sum)) anorm = sum;
            
            for (int i = 1; i < n-1; ++i) {
                sum = ABS_Float16(d[i]) + ABS_Float16(e[i]) + ABS_Float16(e[i-1]);
                if (sum > anorm || LAPACK_HFISNAN(sum)) anorm = sum;
            }
        }
    } 
    else if (lsame_reimpl(norm, 'F') || lsame_reimpl(norm, 'E')) {
        /* Norma Frobenius */
        _Float16 scale = 0.0F16;
        _Float16 sum = 1.0F16;
        
        if (n > 1) {
            for (int i = 0; i < n-1; ++i) {
                _Float16 absxi = ABS_Float16(e[i]);
                if (absxi != 0.0F16) {
                    if (scale < absxi) {
                        sum = 1.0F16 + sum * (scale/absxi) * (scale/absxi);
                        scale = absxi;
                    } else {
                        sum += (absxi/scale) * (absxi/scale);
                    }
                }
            }
            sum *= 2.0F16;
        }
        
        for (int i = 0; i < n; ++i) {
            _Float16 absxi = ABS_Float16(d[i]);
            if (absxi != 0.0F16) {
                if (scale < absxi) {
                    sum = 1.0F16 + sum * (scale/absxi) * (scale/absxi);
                    scale = absxi;
                } else {
                    sum += (absxi/scale) * (absxi/scale);
                }
            }
        }
        
        anorm = scale * custom_sqrtf16(sum);
    }
    
    return anorm;
}