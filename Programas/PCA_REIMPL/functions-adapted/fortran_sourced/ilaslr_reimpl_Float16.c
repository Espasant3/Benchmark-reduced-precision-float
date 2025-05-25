
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file ilaslr_reimpl_Float16.c
 * \brief Implementación de la función ilaslr_reimpl_hf() para matrices en media precisión (_Float16).
 * 
 * Reimplementación adaptada de LAPACK para trabajar con el tipo _Float16. 
 * Incluye lógica específica para replicar el comportamiento de Fortran.
 */

/**
 * \brief Encuentra el índice de la última fila no nula en una matriz.
 * 
 * \details Esta función analiza las filas de una matriz de tipo \c _Float16 para determinar
 *          cuál es la última fila que contiene al menos un elemento no nulo. El comportamiento
 *          está diseñado para replicar la lógica de Fortran utilizada en LAPACK.
 *
 * \param[in] m    Número de filas de la matriz. Si es 0, retorna inmediatamente -1.
 * \param[in] n    Número de columnas de la matriz. Si es 0, se asume que todas las filas son nulas.
 * \param[in] a    Puntero a la matriz en almacenamiento column-major.
 * \param[in] lda  Leading dimension de la matriz (tamaño de la primera dimensión).
 *
 * \return Índice (basado en 0) de la última fila no nula. Retorna:
 *         - -1 si \c m = 0 (sin filas)
 *         - \c m-1 si la última fila tiene elementos no nulos en la primera o última columna
 *         - 0 si todas las columnas son completamente nulas (comportamiento Fortran)
 *         - El índice máximo encontrado durante el escaneo de columnas
 *
 * \note Versión reimplementada para matrices de media precisión (\c _Float16). \n
 *       Prioriza eficiencia verificando primero las esquinas de la última fila. \n
 *       Si todas las columnas son nulas, retorna 0 (siguiendo convenciones de Fortran). \n
 *       El escaneo se realiza por columnas, buscando en cada una la última fila no nula.
 *
 * \example
 * Ejemplo para matriz 3x3 con última fila no nula en la columna 2:
 * \code
 * _Float16 mat[9] = {1,0,0, 0,1,0, 0,0,1};  // Column-major
 * int idx = ilaslr_reimpl_Float16(3, 3, mat, 3);  // Retorna 2 (tercera fila)
 * \endcode
 */

int ilaslr_reimpl_Float16(int m, int n, _Float16 *a, int lda) {
    const _Float16 ZERO = 0.0f16;
    int return_value = 0;

    // Caso 1: No hay filas (M == 0) → Retorna -1 (0-based).
    if (m == 0) {
        return_value = m - 1;
    } else if (a[m - 1] != ZERO || a[m - 1 + (n - 1) * lda] != ZERO) {
        return_value = m - 1; // Si no hay columnas, todas las filas son nulas.
    } else{
        return_value = 0;
        for (int j = 0; j < n; j++) {
            int i = m - 1;

            while(a[MAX(i, 0) + j * lda] == ZERO && i >= 0) {
                i--;

            }
            return_value = MAX(return_value, i);

        }
    }
    return return_value;
}