#include "../include/lapacke_utils_reimpl.h"

void hfsytrd(char uplo, lapack_int n, _Float16 *a, lapack_int lda, _Float16 *d,
             _Float16 *e, _Float16 *tau, _Float16 *work, lapack_int lwork, lapack_int *info) {
    printf("LAPACKE_hfsytrd: n = %d, lda = %d\n", n, lda);
    // Parámetros y variables locales (cambiados a lapack_int)
    lapack_logical upper = lsame_reimpl(uplo, 'U');
    lapack_logical lquery = (lwork == -1);
    lapack_int i, iinfo, iws, j, kk, ldwork, lwkopt, nb, nbmin, nx;

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
        nx = (lapack_int)MAX(3, ilaenv_reimpl_Float16(3, "SSYTRD", "U", n, -1, -1, -1));
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
        kk = n - (((n - nx + nb - 1) / nb) * nb);
        if (kk < 0) kk = 0;  // Asegurar valores no negativos

        /* Reducir columnas superiores de la matriz */
        for (i = n - nb + 1; i >= kk + 1; i -= nb) {
            /* Reducir columnas i:i+nb-1 a forma tridiagonal y formar matriz W */
            hflatrd(uplo, i + nb - 1, nb, &a[i * lda + i], lda, &e[i], &tau[i], work, lwork);

            /* Actualizar submatriz A(1:i-1,1:i-1) con A := A - V*W^T - W*V^T */
            hfsyr2k(uplo, 'N', i - 1, nb, -1.0F16, &a[(i - 1) * lda], lda, work, lwork, 1.0F16, a, lda);

            /* Copiar elementos superdiagonales y diagonales */
            for (j = i; j < i + nb; j++) {
                a[(j - 1) * lda + j] = e[j - 1];
                d[j] = a[j * lda + j];
            }
        }
        /* Reducir el último o único bloque */
        hfsytd2(uplo, kk, &a[kk * lda + kk], lda, &d[kk], &e[kk], &tau[kk], info);
    } else {
        // Reducción del triángulo inferior
        for (i = 0; i < (n - nx - nb); i += nb) {
            
            hflatrd(uplo, n - i + 1, nb, &a[i * lda + i], lda, 
                    &e[i], &tau[i], work, ldwork);  // LDWORK = ib
            
            // Actualizar submatriz restante

            hfsyr2k(uplo, 'N', n - i - nb + 1, nb, -1.0F16, &a[(i + nb) * lda + i],
                lda, &work[nb], ldwork, 1.0F16, &a[(i + nb) * lda + (i + nb)], lda);
            
            // Copiar elementos subdiagonales y diagonales
            for (j = i; j < i + nb; j++) {
                a[(j + 1) * lda + j] = e[j];
                d[j] = a[j * lda + j];
            }
        }
        
        // Procesar el último bloque con SSYTD2
        lapack_int remaining_size = n - i;
        if (remaining_size >= 1 && remaining_size <= n && lda >= remaining_size) {
            hfsytd2(uplo, remaining_size, &a[i * lda + i], lda, 
                    &d[i], &e[i], &tau[i], &iinfo);
        }
    }
    work[0] = hfroundup_lwork(lwkopt); // Asignar el tamaño del workspace
}