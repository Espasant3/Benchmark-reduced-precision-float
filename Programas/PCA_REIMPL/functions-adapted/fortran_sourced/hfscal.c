#include "lapacke_utils_reimpl.h"

void hfscal(int n, lapack_float sa, lapack_float *sx, int incx) {
    /* Verificación de parámetros */
    if (n <= 0 || incx == 0 || sa == (lapack_float)1.0) return;

    /* Caso para incremento unitario (optimizado) */
    if (incx == 1) {
        const int m = n % 5;
        for (int i = 0; i < m; i++) sx[i] *= sa;
        for (int i = m; i < n; i += 5) {
            sx[i]   *= sa;
            sx[i+1] *= sa;
            sx[i+2] *= sa;
            sx[i+3] *= sa;
            sx[i+4] *= sa;
        }
    } 
    /* Caso general corregido para incrementos negativos */
    else {
        for (int i = 0; i < n; i++) {
            sx[i * incx] *= sa;
        }
    }
}