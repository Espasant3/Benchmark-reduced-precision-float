#include "../include/lapacke_utils_reimpl.h"

void hfsytrd(char uplo, lapack_int n, _Float16 *a, lapack_int lda, _Float16 *d,
             _Float16 *e, _Float16 *tau, _Float16 *work, lapack_int lwork, lapack_int *info) {
    // Parámetros y variables locales (cambiados a lapack_int)
    lapack_logical upper = lsame_reimpl(uplo, 'U');
    lapack_logical lquery = (lwork == -1);
    lapack_int i, iinfo, iws, j, kk, ldwork = 1, lwkopt, nb, nbmin, nx;

    // Comprobar parámetros de entrada
    *info = 0;
    if (!upper && !lsame_reimpl(uplo, 'L')) {
        *info = -1;
    } else if (n < 0) {
        *info = -2;
    } else if (lda < MAX(1, n)) {
        *info = -4;
    } else if (lwork < 1 && !lquery) {
        *info = -9;
    }

    if (*info == 0) { 
        // Consultar espacio de trabajo óptimo y lwkopt
        nb = ilaenv_reimpl_Float16(1, "SSYTRD", " ", n, -1, -1, -1);
        lwkopt = MAX(1, n * nb); 
        work[0] = hfroundup_lwork(lwkopt); // Asignar work[0] aquí también
    }

    if (*info != 0) {
        LAPACKE_xerbla("HFSYTRD", -(*info));
        return;
    } else if (lquery) {
        return;
    }

    if (n == 0) {
        work[0] = 1.0F16;
        return;
    }

    // Calcular tamaño de bloque y workspace
    nx = n;
    iws = 1;
    if (nb > 1 && nb < n) {
        nx = MAX(3, ilaenv_reimpl_Float16(3, "SSYTRD", "U", n, -1, -1, -1));
        if (nx < n) {
            ldwork = n;
            iws = ldwork * nb;
            if (lwork < iws) {
                nb = MAX(lwork / ldwork, 1); 
                nbmin = ilaenv_reimpl_Float16(2, "SSYTRD", "U", n, -1, -1, -1);
                if (nb < nbmin) nx = n;
            }
        } else {
            nx = n;
        }
    } else {
        nb = 1;
    }
    // Reducción a forma tridiagonal
    if (upper) {
        /* Calcular KK equivalente a KK = N - ( ( N-NX+NB-1 ) / NB )*NB */
        kk = n - ((n - nx + nb - 1) / nb) * nb;

        /* Reducir columnas superiores de la matriz */
        for (i = n - nb; i >= kk; i -= nb) {
            /* Reducir columnas i:i+nb-1 a forma tridiagonal y formar matriz W */
            hflatrd(uplo, i + nb, nb, a, lda, e, tau, work, ldwork);

            /* Actualizar submatriz A(1:i-1,1:i-1) con A := A - V*W^T - W*V^T */
            hfsyr2k(uplo, 'N', i, nb, -1.0F16, &a[i * lda], lda, work, ldwork, 1.0F16, a, lda);

            /* Copiar elementos superdiagonales y diagonales */
            for (j = i; j < i + nb; j++) {
                a[(j - 1) + j * lda] = e[j - 1];
                d[j] = a[j + j * lda];
            }
        }
        /* Reducir el último o único bloque */
        hfsytd2(uplo, kk, a, lda, d, e, tau, info);
    } else {
        // Reducción del triángulo inferior
        for (i = 0; i < (n - nx); i += nb) {
            
            hflatrd(uplo, n - i, nb, &a[i + i * lda], lda, 
                    &e[i], &tau[i], work, ldwork); 
            
            // Actualizar submatriz restante

            hfsyr2k(uplo, 'N', n - i - nb, nb, -1.0F16, &a[(i + nb) + i * lda],
                lda, &work[nb], ldwork, 1.0F16, &a[(i + nb) + (i + nb) * lda], lda);
            
            // Copiar elementos subdiagonales y diagonales
            for (j = i; j < i + nb; j++) {
                a[(j + 1) + j * lda] = e[j];
                d[j] = a[j + j * lda];
            }
        }
        
        // Procesar el último bloque con SSYTD2
        hfsytd2(uplo, n-i, &a[i + i * lda], lda, &d[i], &e[i], &tau[i], &iinfo);

    }
    work[0] = hfroundup_lwork(lwkopt); 
    return;
}