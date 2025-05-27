
#include "lapacke_utils_reimpl.h"

/**
 * \file ilaslc_reimpl_half_precision.c
 * \brief Implementación de la función ilaslc para matrices de media precisión (lapack_float).
 */

/**
 * \brief Encuentra el índice de la última columna no nula en una matriz.
 * 
 * \details Esta función examina las columnas de una matriz de tipo \c lapack_float, 
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
 * \note Versión reimplementada para matrices de media precisión (\c lapack_float). \n
 *       Optimizada verificando primero la última columna antes de iterar. \n
 *       El chequeo de columnas se hace en orden inverso (desde la última hacia la primera).
 * 
 * \example
 * Ejemplo para una matriz 3x3 con última columna nula:
 * \code
 * lapack_float mat[9] = {1,0,0, 0,1,0, 0,0,0};  // Column-major
 * int idx = ilaslc_reimpl_half_precision(3, 3, mat, 3);  // Retorna 1 (segunda columna)
 * \endcode
 */

int ilaslc_reimpl_half_precision(int m, int n, lapack_float *a, int lda) {
    const lapack_float ZERO = (lapack_float)0.0;


    int return_value = 0;

    if (n == 0) {
        return n - 1; // Caso n = 0
    } else if(a[(n - 1)*lda] != ZERO || a[(m - 1) + (n - 1) * lda] != ZERO) {
        return n - 1; // Última columna tiene elementos no nulos.

    } else{
        for(return_value = n - 1; return_value >= 0; return_value--) {
            for (int i = 0; i < m; i++) {
                if (a[i + return_value * lda] != ZERO) {
                    return return_value; // Columna no nula encontrada.
                }
            }

        }
    }
    return return_value; // Todas las columnas son nulas.
}