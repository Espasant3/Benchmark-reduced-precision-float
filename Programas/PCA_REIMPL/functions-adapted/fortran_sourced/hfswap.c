
#include "lapacke_utils_reimpl.h"

/**
 * \file hfswap.c
 * \brief Implementación de intercambio (SSWAP) de vectores en lapack_float
 */

/**
 * \brief Intercambia los elementos de dos vectores lapack_float
 * 
 * \details Versión optimizada para lapack_float de BLAS SSWAP. Implementa:
 *          - Caso optimizado para incrementos unitarios con desenrollado de bucle
 *          - Manejo eficiente de incrementos negativos/arbitrarios
 *          - Operación in-place sin uso de memoria temporal adicional
 * 
 * \param[in] n         Número de elementos a intercambiar (n >= 0)
 * \param[in,out] sx    Array lapack_float de origen/destino (tamaño >= |incx|*(n-1) + 1)
 * \param[in] incx      Incremento entre elementos en sx (0 para n=0, ≠0 para n>0)
 * \param[in,out] sy    Array lapack_float de origen/destino (tamaño >= |incy|*(n-1) + 1)
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
 * lapack_float x[5] = {1.0F16, 2.0F16, 3.0F16, 4.0F16, 5.0F16};
 * lapack_float y[5] = {6.0F16, 7.0F16, 8.0F16, 9.0F16, 10.0F16};
 * 
 * // Intercambiar primeros 3 elementos con incremento 2
 * hfswap(3, x, 2, y, 2);  // x[0,2,4] ↔ y[0,2,4]
 * 
 * \see BLAS SSWAP Para la versión en precisión simple
 * \see _Float16, __fp16, __bf16 Para detalles del tipo de datos
 */

void hfswap(int n, lapack_float *sx, int incx, lapack_float *sy, int incy) {
    if (n <= 0) return;

    if (incx == 1 && incy == 1) {
        // Caso optimizado para incrementos unitarios
        int m = n % 3;
        if (m != 0) {
            for (int i = 0; i < m; i++) {
                lapack_float temp = sx[i];
                sx[i] = sy[i];
                sy[i] = temp;
            }
            if (n < 3) return;
        }
        // Bucle desenrollado para bloques de 3 elementos
        for (int i = m; i < n; i += 3) {
            lapack_float temp;
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
            lapack_float temp = sx[ix];
            sx[ix] = sy[iy];
            sy[iy] = temp;

            ix += incx;
            iy += incy;
        }
    }
}