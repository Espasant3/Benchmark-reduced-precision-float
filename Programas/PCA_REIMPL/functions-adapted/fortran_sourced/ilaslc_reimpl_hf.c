
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file ilaslc_reimpl_hf.c
 * \brief Implementación de la función ilaslc para matrices de media precisión (_Float16).
 */

/**
 * \brief Encuentra el índice de la última columna no nula en una matriz.
 * 
 * \details Esta función examina las columnas de una matriz de tipo \c _Float16, 
 *          empezando desde la última columna, para determinar cuál es la última 
 *          que contiene al menos un elemento no nulo. Si todas las columnas son nulas
 *          o la matriz tiene 0 columnas, retorna -1.
 *
 * \param[in] m    Número de filas de la matriz. Debe ser mayor o igual a 0.
 * \param[in] n    Número de columnas de la matriz. Si es 0, retorna inmediatamente -1.
 * \param[in] a    Puntero a la matriz en memoria (almacenamiento column-major).
 * \param[in] lda  Leading dimension de la matriz (tamaño de la primera dimensión).
 * 
 * \return Índice (basado en 0) de la última columna no nula. \n
 *         -1 si:
 *         - La matriz tiene 0 columnas (\c n = 0)
 *         - Todas las columnas son completamente nulas
 * 
 * \note Versión reimplementada para matrices de media precisión (\c _Float16). \n
 *       Optimizada verificando primero la última columna antes de iterar. \n
 *       El chequeo de columnas se hace en orden inverso (desde la última hacia la primera).
 * 
 * \example
 * Ejemplo para una matriz 3x3 con última columna nula:
 * \code
 * _Float16 mat[9] = {1,0,0, 0,1,0, 0,0,0};  // Column-major
 * int idx = ilaslc_reimpl_hf(3, 3, mat, 3);  // Retorna 1 (segunda columna)
 * \endcode
 */

int ilaslc_reimpl_hf(int m, int n, _Float16 *a, int lda) {
    const _Float16 ZERO = 0.0f16;
    
    if (n == 0) {
        return -1; // Caso n = 0
    }

    int last_col = n - 1;
    if (a[last_col * lda] != ZERO || a[(m-1) + last_col * lda] != ZERO) {
        return last_col; // Última columna tiene elementos no nulos.
    }

    for (int j = last_col; j >= 0; j--) {
        for (int i = 0; i < m; i++) {
            if (a[i + j * lda] != ZERO) {
                return j; // Columna no nula encontrada.
            }
        }
    }

    return -1; // Todas las columnas son nulas.
}