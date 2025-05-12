
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file hforgqr.c
 * \brief Implementación de generación de matriz Q ortogonal en _Float16
 */

/**
 * \brief Genera la matriz Q completa de una factorización QR en _Float16
 * 
 * \details Versión _Float16 de LAPACK's SORGQR. Genera las primeras n columnas de Q
 *          a partir de una factorización QR computada por hgeqrf.
 * 
 * \param[in] m     Filas de la matriz A (_Float16[m][lda])
 * \param[in] n     Columnas de Q a generar (0 <= n <= m)
 * \param[in] k     Número de reflectores elementales de Householder (0 <= k <= n)
 * \param[in,out] a Matriz _Float16 column-major (lda >= m)
 * \param[in] lda   Leading dimension de A 
 * \param[in] tau   Factores escalares _Float16 de los reflectores
 * \param[out] work Workspace _Float16 (tamaño >= max(1, lwork))
 * \param[in] lwork Dimensión del workspace (usar -1 para query)
 * \param[out] info Código de retorno (0 = éxito)
 * 
 * \note
 * - Implementación específica para _Float16 (IEEE 754-2008 binary16)
 * - Diferencias clave vs __fp16:
 *   + _Float16 tiene garantizada la aritmética IEEE
 *   + __fp16 es solo tipo almacenamiento en algunas arquitecturas
 * 
 * \warning
 * - Requiere soporte hardware/emulación de operaciones _Float16
 * - No compatible con tipos half/floats16 de otros estándares
 */

void hforgqr(int m, int n, int k, _Float16 *a, int lda, _Float16 *tau, _Float16 *work, int lwork, int *info) {
    int nb, lwkopt, lquery;
    int nbmin, nx, iws, ldwork;
    int ki, kk, i, ib, j, l, iinfo;
    int zero = 0;

    *info = 0;
    nb = ilaenv_reimpl_Float16(1, "SORGQR", " ", m, n, k, zero);
    lwkopt = (n > 0) ? n * nb : 1;
    work[0] = hfroundup_lwork(lwkopt);
    lquery = (lwork == -1);

    if (m < 0) {
        *info = -1;
    } else if (n < 0 || n > m) {
        *info = -2;
    } else if (k < 0 || k > n) {
        *info = -3;
    } else if (lda < (m > 0 ? m : 1)) {
        *info = -5;
    } else if (lwork < ((n > 0) ? n : 1) && !lquery) {
        *info = -8;
    }

    if (*info != 0) {
        LAPACKE_xerbla("HFORGQR", *info);
        return;
    } else if (lquery) {
        return;
    }

    if (n <= 0) {
        work[0] = 1.0F16;
        return;
    }

    nbmin = 2;
    nx = 0;
    iws = n;
    if (nb > 1 && nb < k) {
        nx = ilaenv_reimpl_Float16(3, "SORGQR", " ", m, n, k, zero);
        nx = (nx > 0) ? nx : 0;
        if (nx < k) {
            ldwork = n;
            iws = ldwork * nb;
            if (lwork < iws) {
                nb = lwork / ldwork;
                nbmin = ilaenv_reimpl_Float16(2, "SORGQR", " ", m, n, k, zero);
                nbmin = (nbmin > 2) ? nbmin : 2;
            }
        }
    }

    if (nb >= nbmin && nb < k && nx < k) {
        ki = (k - nx - 1) / nb * nb;
        kk = (k < ki + nb) ? k : ki + nb;

        for (j = kk; j < n; j++) {
            for (i = 0; i < kk; i++) {
                a[j * lda + i] = 0.0F16;
            }
        }
    } else {
        kk = 0;
    }

    if (kk < n) {
        int mm = m - kk;
        int nn = n - kk;
        int kk2 = k - kk;
        hforg2r(mm, nn, kk2, &a[kk * lda + kk], lda, &tau[kk], work, &iinfo);
    }

    if (kk > 0) {
        for (i = ki; i >= 0; i -= nb) {
            ib = (k - i) < nb ? (k - i) : nb;
            if (i + ib <= n) {
                int m_i = m - i;
                int n_ib = n - i - ib;
                int lwork2 = n - i - ib;

                hflarft('F', 'C', m_i, ib, &a[i * lda + i], lda, &tau[i], work, ib);
                hflarfb('L', 'N', 'F', 'C', m_i, n_ib, ib,
                        &a[i * lda + i], lda, work, ib, &a[i * lda + (i + ib)], lda,
                        &work[ib], lwork2);
            }

            int m_i = m - i;
            hforg2r(m_i, ib, ib, &a[i * lda + i], lda, &tau[i], work, &iinfo);

            for (j = i; j < i + ib; j++) {
                for (l = 0; l < i; l++) {
                    a[j * lda + l] = 0.0F16;
                }
            }
        }
    }

    work[0] = hfroundup_lwork(iws);
}