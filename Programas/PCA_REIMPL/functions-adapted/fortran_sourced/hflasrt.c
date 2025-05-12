
#include "../include/lapacke_utils_reimpl.h"

void hflasrt(char id, lapack_int n, _Float16 *d, lapack_int *info) {
    const int SELECT = 20;
    int dir = -1;
    int stack[32][2];
    int stkpnt = 0;
    int start, endd, i, j, mid;
    _Float16 d1, d2, d3, dmnmx, tmp;

    *info = 0;

    if (lsame_reimpl(id, 'D')) {
        dir = 0;
    } else if (lsame_reimpl(id, 'I')) {
        dir = 1;
    } else {
        *info = -1;
        LAPACKE_xerbla("hflasrt", -(*info));
        return;
    }

    // Validar parámetro N
    if (n < 0) {
        *info = -2;
        LAPACKE_xerbla("hflasrt", -(*info));
        return;
    }

    // Caso trivial
    if (n <= 1) return;

    // Inicializar pila con el rango completo (0-based)
    stack[0][0] = 0;
    stack[0][1] = n - 1;
    stkpnt = 1;

    while (stkpnt > 0) {
        // Extraer partición de la pila
        stkpnt--;
        start = stack[stkpnt][0];
        endd = stack[stkpnt][1];

        // Insertion sort para particiones pequeñas
        if (endd - start <= SELECT && endd > start) {
            if (dir == 0) { // Orden descendente
                for (i = start + 1; i <= endd; i++) {
                    for (j = i; j > start; j--) {
                        if (d[j] > d[j-1]) {
                            tmp = d[j];
                            d[j] = d[j-1];
                            d[j-1] = tmp;
                        } else {
                            break;
                        }
                    }
                }
            } else { // Orden ascendente
                for (i = start + 1; i <= endd; i++) {
                    for (j = i; j > start; j--) {
                        if (d[j] < d[j-1]) {
                            tmp = d[j];
                            d[j] = d[j-1];
                            d[j-1] = tmp;
                        } else {
                            break;
                        }
                    }
                }
            }
        } else if (endd - start > SELECT) {
            // Selección del pivote (mediana de 3)
            d1 = d[start];
            d2 = d[endd];
            mid = (start + endd) / 2;
            d3 = d[mid];

            if (d1 < d2) {
                if (d3 < d1) dmnmx = d1;
                else if (d3 < d2) dmnmx = d3;
                else dmnmx = d2;
            } else {
                if (d3 < d2) dmnmx = d2;
                else if (d3 < d1) dmnmx = d3;
                else dmnmx = d1;
            }

            // Partición
            i = start - 1;
            j = endd + 1;
            if (dir == 0) { // Descendente
                while (1) {
                    do { j--; } while (d[j] < dmnmx);
                    do { i++; } while (d[i] > dmnmx);
                    if (i < j) {
                        tmp = d[i];
                        d[i] = d[j];
                        d[j] = tmp;
                    } else {
                        break;
                    }
                }
            } else { // Ascendente
                while (1) {
                    do { j--; } while (d[j] > dmnmx);
                    do { i++; } while (d[i] < dmnmx);
                    if (i < j) {
                        tmp = d[i];
                        d[i] = d[j];
                        d[j] = tmp;
                    } else {
                        break;
                    }
                }
            }

            // Apilar particiones (la más grande primero)
            if (stkpnt < 31) {
                if (j - start > endd - j - 1) {
                    stack[stkpnt][0] = start;
                    stack[stkpnt][1] = j;
                    stkpnt++;
                    stack[stkpnt][0] = j + 1;
                    stack[stkpnt][1] = endd;
                    stkpnt++;
                } else {
                    stack[stkpnt][0] = j + 1;
                    stack[stkpnt][1] = endd;
                    stkpnt++;
                    stack[stkpnt][0] = start;
                    stack[stkpnt][1] = j;
                    stkpnt++;
                }
            }
        }
    }
}