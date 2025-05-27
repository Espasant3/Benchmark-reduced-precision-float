#ifndef PCA_REIMPL_H
#define PCA_REIMPL_H

#include "lapacke_utils_reimpl.h"


// LAPACKE_hfsyev - LAPACKE_ssyev implemented for lapack_float

/**
 * \brief Calcula autovalores y (opcionalmente) autovectores de una matriz simétrica en lapack_float
 * 
 * \details Versión lapack_float de LAPACK's SSYEV. Resuelve el problema:
 *          \f$ A \cdot V = V \cdot \Lambda \f$
 *          donde:
 *          - A es simétrica (almacenada triangular superior/inferior)
 *          - Λ diagonal de autovalores
 *          - V matriz ortogonal de autovectores (si jobz='V')
 * 
 * \param[in] matrix_layout Esquema de almacenamiento:
 *                        - LAPACK_COL_MAJOR: Column-major (Fortran-style)
 *                        - LAPACK_ROW_MAJOR: Row-major (C-style)
 * \param[in] jobz        'N': Solo autovalores; 'V': Autovalores y autovectores
 * \param[in] uplo        'U': Triángulo superior almacenado; 'L': Triángulo inferior
 * \param[in] n           Orden de la matriz A (n >= 0)
 * \param[in,out] a       Matriz lapack_float (lda, n). En salida contiene los autovectores si jobz='V'
 * \param[in] lda         Leading dimension de A (lda >= max(1,n))
 * \param[out] w          Array lapack_float de autovalores (tamaño n), orden ascendente
 * 
 * \return Código de salida:
 *         - 0: éxito
 *         - <0: error en parámetro i
 *         - >0: error en algoritmo
 * 
 * \note
 * - Verificación opcional de NaN controlada por LAPACKE_NANCHECK
 * - Asignación dinámica de memoria para workspace óptimo
 * - Compatible con matrices row-major mediante transformación interna
 * 
 */

lapack_int LAPACKE_hfsyev( int matrix_layout, char jobz, char uplo, lapack_int n,
    lapack_float* a, lapack_int lda, lapack_float* w );

#endif /* PCA_REIMPL_H */