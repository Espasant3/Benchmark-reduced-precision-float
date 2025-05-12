#include "../include/lapacke_utils_reimpl.h"

void hfscal(int n, _Float16 sa, _Float16 *sx, int incx) {
    int i, m;

    /* Verificación de parámetros */
    if (n <= 0 || incx <= 0 || sa == 1.0F16) {
        return;
    }

    /* Caso para incremento unitario */
    if (incx == 1) {
        m = n % 5;
        
        /* Procesar elementos residuales */
        for (i = 0; i < m; i++) {
            sx[i] = sx[i] * sa;
        }
        
        /* Salir si el tamaño es menor al bloque de desenrollado */
        if (n < 5) return;
        
        /* Bucle desenrollado x5 */
        for (i = m; i < n; i += 5) {
            sx[i]   = sx[i] * sa;
            sx[i+1] = sx[i+1] * sa;
            sx[i+2] = sx[i+2] * sa;
            sx[i+3] = sx[i+3] * sa;
            sx[i+4] = sx[i+4] * sa;
        }
    } 
    else {
        /* Caso para incremento no unitario */
        for (i = 0; i < n * incx; i += incx) {
            sx[i] = sx[i] * sa;
        }
    }
}