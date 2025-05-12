
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file ilaslr_reimpl_hf.c
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
 * int idx = ilaslr_reimpl_hf(3, 3, mat, 3);  // Retorna 2 (tercera fila)
 * \endcode
 */

int ilaslr_reimpl_hf(int m, int n, _Float16 *a, int lda) {
    const _Float16 ZERO = 0.0f16;

    // Caso 1: No hay filas (M == 0) → Retorna -1 (0-based).
    if (m == 0) {
        return -1;
    }

    int last_row = m - 1;

    // Caso 2: Verificar esquinas de la última fila (0-based).
    if (n > 0) {
        if (a[last_row + 0 * lda] != ZERO || a[last_row + (n-1) * lda] != ZERO) {
            return last_row;
        }
    }

    // Caso 3: Escanear columnas.
    int max_nonzero_row = -1;
    for (int j = 0; j < n; j++) {
        int i = last_row;

        // Encontrar la última fila no nula en la columna j.
        while (i >= 0 && a[i + j * lda] == ZERO) {
            i--;
        }

        // Ajustar i a 0 si toda la columna es cero (como en Fortran).
        if (i < 0) {
            i = 0; // ¡Clave! Replicar el comportamiento de Fortran.
        }

        if (i > max_nonzero_row) {
            max_nonzero_row = i;
        }
    }

    // Si todas las columnas son cero, retorna 0 (como en Fortran).
    return (max_nonzero_row == -1) ? 0 : max_nonzero_row;
}