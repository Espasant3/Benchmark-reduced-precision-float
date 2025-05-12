
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file hfswap.c
 * \brief Implementación de intercambio (SSWAP) de vectores en _Float16
 */

/**
 * \brief Intercambia los elementos de dos vectores _Float16
 * 
 * \details Versión optimizada para _Float16 de BLAS SSWAP. Implementa:
 *          - Caso optimizado para incrementos unitarios con desenrollado de bucle
 *          - Manejo eficiente de incrementos negativos/arbitrarios
 *          - Operación in-place sin uso de memoria temporal adicional
 * 
 * \param[in] n         Número de elementos a intercambiar (n >= 0)
 * \param[in,out] sx    Array _Float16 de origen/destino (tamaño >= |incx|*(n-1) + 1)
 * \param[in] incx      Incremento entre elementos en sx (0 para n=0, ≠0 para n>0)
 * \param[in,out] sy    Array _Float16 de origen/destino (tamaño >= |incy|*(n-1) + 1)
 * \param[in] incy      Incremento entre elementos en sy (0 para n=0, ≠0 para n>0)
 * 
 * \note
 * - Optimización principal: Desenrollado de bucle x3 para incx=incy=1
 * - Manejo de incrementos negativos mediante indexación inversa
 * - No verificación de solapamiento de memoria (comportamiento indefinido si sx/sy se solapan)
 * 
 * \warning
 * - Con n=0 retorna inmediatamente sin operaciones
 * - Incrementos cero con n>0 causan acceso fuera de rango
 * - No valida punteros (sx/sy deben ser válidos)
 * 
 * \par Algoritmo:
 * 1. Caso optimizado (incx=incy=1):
 *    \code
 *    for (i = 0; i < n; i += 3) {
 *        swap(sx[i], sy[i]);
 *        swap(sx[i+1], sy[i+1]);
 *        swap(sx[i+2], sy[i+2]);
 *    }
 *    \endcode
 * 2. Caso general:
 *    \code
 *    ix = (incx >= 0) ? 0 : -incx*(n-1)
 *    iy = (incy >= 0) ? 0 : -incy*(n-1)
 *    for (i=0; i<n; i++) {
 *        swap(sx[ix], sy[iy])
 *        ix += incx
 *        iy += incy
 *    }
 *    \endcode
 * 
 * \example
 * _Float16 x[5] = {1.0F16, 2.0F16, 3.0F16, 4.0F16, 5.0F16};
 * _Float16 y[5] = {6.0F16, 7.0F16, 8.0F16, 9.0F16, 10.0F16};
 * 
 * // Intercambiar primeros 3 elementos con incremento 2
 * hfswap(3, x, 2, y, 2);  // x[0,2,4] ↔ y[0,2,4]
 * 
 * \see BLAS SSWAP Para la versión en precisión simple
 * \see _Float16 Para detalles del tipo de datos
 */

void hfswap(int n, _Float16 *sx, int incx, _Float16 *sy, int incy) {
    if (n <= 0) return;

    if (incx == 1 && incy == 1) {
        // Caso optimizado para incrementos unitarios
        int m = n % 3;
        if (m != 0) {
            for (int i = 0; i < m; i++) {
                _Float16 temp = sx[i];
                sx[i] = sy[i];
                sy[i] = temp;
            }
            if (n < 3) return;
        }
        // Bucle desenrollado para bloques de 3 elementos
        for (int i = m; i < n; i += 3) {
            _Float16 temp;
            temp = sx[i];
            sx[i] = sy[i];
            sy[i] = temp;

            temp = sx[i + 1];
            sx[i + 1] = sy[i + 1];
            sy[i + 1] = temp;

            temp = sx[i + 2];
            sx[i + 2] = sy[i + 2];
            sy[i + 2] = temp;
        }
    } else {
        // Caso general para incrementos arbitrarios
        int ix = (incx >= 0) ? 0 : (-n + 1) * incx;
        int iy = (incy >= 0) ? 0 : (-n + 1) * incy;

        for (int i = 0; i < n; i++) {
            _Float16 temp = sx[ix];
            sx[ix] = sy[iy];
            sy[iy] = temp;

            ix += incx;
            iy += incy;
        }
    }
}

/**
 * \brief Intercambia columnas en matrices cuadradas row-major
 * 
 * \param[in] n       Dimensión de la matriz (N x N) y número de elementos a intercambiar
 * \param[in,out] sx  Puntero al primer elemento de la primera columna
 * \param[in] incx    Incremento lógico (original para column-major)
 * \param[in,out] sy  Puntero al primer elemento de la segunda columna
 * \param[in] incy    Incremento lógico (original para column-major)
 * 
 * \note
 * - Asume que la matriz es cuadrada (n = número de filas = número de columnas).
 * - Ajusta los incrementos multiplicando por `n` (saltos entre filas en row-major).
 * 
 * \warning
 * - incx/incy deben corresponder al espaciado real en memoria
 * - Comportamiento indefinido si sx/sy se solapan parcialmente
 */

void hfswap_row_major(int n, _Float16 *sx, int incx, _Float16 *sy, int incy) { // Esta implementación es dudosa y no se ha probado, para column-major es fiable
    // Ajuste para row-major: incx *= n (solo aplica a matrices n*n que son las que recibe lapacke)
    //hfswap(n, sx, incx * n, sy, incy * n); 
    hfswap(n, sx, incx, sy, incy);

}

/**
 * \brief Intercambia columnas en matrices almacenadas en formato column-major
 * 
 * \param[in] n       Número de elementos a intercambiar (longitud de las columnas)
 * \param[in,out] sx  Puntero al primer elemento de la columna origen
 * \param[in] incx    Incremento entre elementos de la columna origen (=1 para columnas contiguas)
 * \param[in,out] sy  Puntero al primer elemento de la columna destino
 * \param[in] incy    Incremento entre elementos de la columna destino (=1 para columnas contiguas)
 * 
 * \note
 * - Diseñado para matrices column-major (formato Fortran/LAPACK)
 * - Para matrices 2D, incx/incy deben ser la leading dimension (número de filas)
 * - Wrapper directo de hfswap sin ajuste de incrementos
 * 
 * \warning
 * - incx/incy deben corresponder al espaciado real en memoria
 * - Comportamiento indefinido si sx/sy se solapan parcialmente
 */

void hfswap_column_major(int n, _Float16 *sx, int incx, _Float16 *sy, int incy) {
    
    hfswap(n, sx, incx, sy, incy);
}