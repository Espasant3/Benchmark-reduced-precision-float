
#include "../include/lapacke_utils_reimpl.h"

void hflasrt(char id, lapack_int n, _Float16 *d, lapack_int *info) {
    const int SELECT = 20;
    int dir;
    int stack[2][32];
    int stkpnt = 0;
    int start, endd, i, j;
    int went_back_full = 1;
    _Float16 d1, d2, d3, dmnmx, tmp;

    *info = 0;
    dir = -1;

    if (lsame_reimpl(id, 'D')) {
        dir = 0;
    } else if (lsame_reimpl(id, 'I')) {
        dir = 1;
    } 
    if(dir == -1) {
        *info = -1;
    } else if(n < 0){
        *info = -2;
    }
    if(*info != 0) {
        LAPACKE_xerbla("hflasrt", -(*info));
        return;
    }
    
    // Caso trivial
    if (n <= 1) return;

    // Inicializar pila con el rango completo (0-based)
    
    stkpnt = 0;
    stack[0][0] = 0;
    stack[0][1] = n - 1;

    do{
        // Extraer partición de la pila
        start = stack[0][stkpnt];
        endd = stack[1][stkpnt];
        stkpnt--;

        // Insertion sort para particiones pequeñas
        if (endd - start <= SELECT && endd - start > 0) {
            if (dir == 0) { // Orden descendente
                for (i = start; i < endd; i++) {
                    for (j = i; j >= start + 1; j--) {
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
                for (i = start; i < endd; i++) {
                    for (j = i; j >= start + 1; j--) {
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
            i = (start + endd) / 2;
            d3 = d[i];

            if (d1 < d2) {
                if (d3 < d1) dmnmx = d1;
                else if (d3 < d2) dmnmx = d3;
                else dmnmx = d2;
            } else {
                if (d3 < d2) dmnmx = d2;
                else if (d3 < d1) dmnmx = d3;
                else dmnmx = d1;
            }

            went_back_full = 1;
            if (dir == 0) { // Descendente

                i = start - 1;
                j = endd + 1;

                while (1) {
                    if(went_back_full){
                        j--;
                        if(d[j] < dmnmx){
                            continue;
                        }
                    }
                    went_back_full = 1;
                    i++;

                    if(d[i] > dmnmx){
                        went_back_full = 0;
                        continue;
                    }
                    if(i < j){
                        tmp = d[i];
                        d[i] = d[j];
                        d[j] = tmp;
                        went_back_full = 1;
                        continue;
                    }
                    if(j - start > endd - j - 1){
                        stkpnt++;
                        stack[0][stkpnt] = start;
                        stack[1][stkpnt] = j;
                        stkpnt++;
                        stack[0][stkpnt] = j + 1;
                        stack[1][stkpnt] = endd;
                        stkpnt++;
                    } else{
                        stkpnt++;
                        stack[0][stkpnt] = j + 1;
                        stack[1][stkpnt] = endd;
                        stkpnt++;
                        stack[0][stkpnt] = start;
                        stack[1][stkpnt] = j;
                        stkpnt++;
                    }
                    break;
                }
            } else { // Ascendente

                i = start - 1;
                j = endd + 1;
                while (1) {
                    if(went_back_full){
                        j--;
                        if(d[j] < dmnmx){
                            went_back_full = 1;
                            continue;
                        }
                    }
                    went_back_full = 1;
                    i++;

                    if(d[j] < dmnmx){
                        went_back_full = 0;
                        continue;
                    }
                    if(i < j){
                        tmp = d[i];
                        d[i] = d[j];
                        d[j] = tmp;
                        went_back_full = 1;
                        continue;
                    }
                    if(j - start > endd - j - 1){
                        stkpnt++;
                        stack[0][stkpnt] = start;
                        stack[1][stkpnt] = j;
                        stkpnt++;
                        stack[0][stkpnt] = j + 1;
                        stack[1][stkpnt] = endd;
                    } else{
                        stkpnt++;
                        stack[0][stkpnt] = j + 1;
                        stack[1][stkpnt] = endd;
                        stkpnt++;
                        stack[0][stkpnt] = start;
                        stack[1][stkpnt] = j;
                    }
                    break;
                }
            }
        }
        /*
        if(stkpnt < 0){
            break;
        }
        */
    }while (stkpnt >= 0);
}