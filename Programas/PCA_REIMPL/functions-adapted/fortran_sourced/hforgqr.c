
#include "lapacke_utils_reimpl.h"

/**
 * \file hforgqr.c
 * \brief Implementación de generación de matriz Q ortogonal en lapack_float
 */

/**
 * \brief Genera la matriz Q completa de una factorización QR en lapack_float
 * 
 * \details Versión lapack_float de LAPACK's SORGQR. Genera las primeras n columnas de Q
 *          a partir de una factorización QR computada por hgeqrf.
 * 
 * \param[in] m     Filas de la matriz A (lapack_float[m][lda])
 * \param[in] n     Columnas de Q a generar (0 <= n <= m)
 * \param[in] k     Número de reflectores elementales de Householder (0 <= k <= n)
 * \param[in,out] a Matriz lapack_float column-major (lda >= m)
 * \param[in] lda   Leading dimension de A 
 * \param[in] tau   Factores escalares lapack_float de los reflectores
 * \param[out] work Workspace lapack_float (tamaño >= max(1, lwork))
 * \param[in] lwork Dimensión del workspace (usar -1 para query)
 * \param[out] info Código de retorno (0 = éxito)
 * 
 * \note
 * - Implementación específica para lapack_float (IEEE 754-2008 binary16)
 * - Diferencias clave vs __fp16:
 *   + lapack_float tiene garantizada la aritmética IEEE
 *   + __fp16 es solo tipo almacenamiento en algunas arquitecturas
 * 
 * \warning
 * - Requiere soporte hardware/emulación de operaciones lapack_float
 * - No compatible con tipos half/floats16 de otros estándares
 */

void hforgqr(int m, int n, int k, lapack_float *a, int lda, lapack_float *tau, lapack_float *work, int lwork, int *info) {
    int nb, lwkopt, lquery;
    int nbmin, nx, iws, ldwork;
    int ki, kk, i, ib, j, l, iinfo;
    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;

    *info = 0;
    nb = ilaenv_reimpl_half_precision(1, "SORGQR", " ", m, n, k, -1);
    lwkopt = MAX(1, n)*nb;
    work[0] = hfroundup_lwork(lwkopt);
    lquery = (lwork == -1);

    if (m < 0) {
        *info = -1;
    } else if (n < 0 || n > m) {
        *info = -2;
    } else if (k < 0 || k > n) {
        *info = -3;
    } else if (lda < MAX(1, m)) {
        *info = -5;
    } else if (lwork < MAX(1,n) && !lquery) {
        *info = -8;
    }

    if (*info != 0) {
        LAPACKE_xerbla("HFORGQR", -(*info));
        return;
    } else if (lquery) {
        return;
    }

    if (n <= 0) {
        work[0] = ONE;
        return;
    }

    nbmin = 2;
    nx = 0;
    iws = n;
    if (nb > 1 && nb < k) {
        nx = MAX(0, ilaenv_reimpl_half_precision(3, "SORGQR", " ", m, n, k, -1));
        if (nx < k) {
            ldwork = n;
            iws = ldwork * nb;
            if (lwork < iws) {
                nb = lwork / ldwork;
                nbmin = MAX(2, ilaenv_reimpl_half_precision(2, "SORGQR", " ", m, n, k, -1));
            }
        }
    }

    if (nb >= nbmin && nb < k && nx < k) {
        ki = ((k - nx - 1) / nb) * nb;
        kk = MIN(k, ki + nb);

        for (j = kk; j < n; j++) {
            for (i = 0; i < kk; i++) {
                a[i + j * lda] = ZERO;
            }
        }
    } else {
        kk = 0;
    }

    if (kk < n) {
        hforg2r(m - kk, n - kk, k - kk, &a[kk + kk * lda], lda, &tau[kk], work, &iinfo);
    }

    if (kk > 0) {
        for (i = ki; i >= 0; i -= nb) {
            ib = MIN(nb, k - i + 1);
            if ((i + 1) + ib <= n) {

                hflarft('F', 'C', m - i, ib, &a[i + i * lda], lda, &tau[i], work, ldwork);
                hflarfb('L', 'N', 'F', 'C', m - i, n - i - ib, ib,
                        &a[i + i * lda], lda, work, ldwork, &a[i + (i + ib) * lda], lda,
                        &work[ib], ldwork);
            }

            hforg2r(m - i, ib, ib, &a[i + i * lda], lda, &tau[i], work, &iinfo);

            for (j = i; j < i + ib - 1; j++) {
                for (l = 0; l < i - 1; l++) {
                    a[j + l * lda] = ZERO;
                }
            }
        }
    }

    work[0] = hfroundup_lwork(iws);
}