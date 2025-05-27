
#include "lapacke_utils_reimpl.h"

/**
 * \file hforg2r.c
 * \brief Genera una matriz ortogonal Q a partir de una factorización QR en precisión media (lapack_float).
 */

/**
 * \brief Genera una matriz ortogonal Q a partir de reflectores Householder en precisión media (lapack_float).
 * 
 * \param[in] m      Número de filas de la matriz A.
 * \param[in] n      Número de columnas de la matriz A (debe cumplir 0 <= n <= m).
 * \param[in] k      Número de reflectores Householder utilizados (0 <= k <= n).
 * \param[in,out] A  Matriz de tamaño lda x n. 
 *                   - Entrada: Contiene los reflectores Householder en las primeras k columnas.
 *                   - Salida: Almacena la matriz ortogonal Q en formato column-major.
 * \param[in] lda    Leading dimension de la matriz A (debe ser >= max(1, m)).
 * \param[in] tau    Vector de factores escalares de los reflectores (longitud >= k).
 * \param[out] work  Vector de trabajo (longitud >= n).
 * \param[out] info  Código de salida:
 *                   - 0: Ejecución exitosa
 *                   - <0: Error en el parámetro de entrada (-i indica el i-ésimo parámetro inválido).
 * 
 * \note
 * - Esta versión utiliza el tipo lapack_float para operaciones en media precisión.
 * - Inicializa las columnas desde k hasta n-1 con la matriz identidad.
 * - Aplica los reflectores Householder en orden inverso para construir Q.
 * - Las funciones auxiliares hflarf1f() y hfscal() deben soportar lapack_float.
 * 
 * \warning
 * - Los literales flotantes usan el sufijo `f16` (requiere soporte del compilador).
 * - La precisión reducida de lapack_float puede afectar la estabilidad numérica.
 * - Las dimensiones de la matriz deben cumplir las restricciones especificadas.
 * 
 * \see LAPACKE_xerbla
 * \see hflarf1f
 * \see hfscal
 */

void hforg2r(int m, int n, int k, lapack_float *A, int lda, lapack_float *tau, lapack_float *work, int *info) {
   
    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;

    *info = 0;

    // Validación de parámetros
    if (m < 0) {
        *info = -1;
    } else if (n < 0 || n > m) {
        *info = -2;
    } else if (k < 0 || k > n) {
        *info = -3;
    } else if (lda < MAX(1, m)) {
        *info = -5;
    }
    if (*info != 0) {
        LAPACKE_xerbla("hforg2r", -(*info));
        return;
    }

    if (n <= 0) return;

    // Inicializar columnas k..n-1 como identidad
    for (int j = k; j < n; j++) {
        for (int l = 0; l < m; l++) {
            A[l + j * lda] = ZERO;
        }
        A[j + j * lda] = ONE;
    }
    // --- Paso 2: Aplicar reflectores en orden inverso (k-1 a 0) ---
    // Aplicar reflectores en orden inverso
    for (int i = k - 1; i >= 0; i--) {

        if(i < n){
            hflarf1f('L', m-i, n-i-1, &A[i + i*lda], 1, tau[i], &A[i + (i+1)*lda], lda, work);
        }
        if(i < m){
            hfscal(m-i-1, -tau[i], &A[i+1 + i*lda], 1);
        }
        A[i + i*lda] = ONE - tau[i];

        for(int l = 0; l < i - 1; l++) {
            A[l + i*lda] = ZERO;
        }
    }
    return;
}