/*
 * Adaptado de LAPACK (netlib.org/lapack) para media precisión
 * 
 * Copyright original:
 *   Copyright (c) 1992-2025 The University of Tennessee and The University
 *                        of Tennessee Research Foundation. All rights reserved.
 *   Copyright (c) 2000-2025 The University of California Berkeley. All rights reserved.
 *   Copyright (c) 2006-2025 The University of Colorado Denver. All rights reserved.
 * 
 * Modificaciones (c) 2025 Eloi Barcón Piñeiro
 * 
 * Licencia: BSD modificada (ver ../../../../LICENSE_LAPACK)
 */

#include "lapacke_utils_reimpl.h"

void hflarft(char direct, char storev, int n, int k, lapack_float *v, int ldv, lapack_float *tau, lapack_float *t, int ldt) {
    /* Base cases */
    if (n == 0 || k == 0) return;
    if (n == 1 || k == 1) {
        t[0] = tau[0];
        return;
    }

    const lapack_float ONE = (lapack_float) 1.0;
    int l = k / 2;
    int kl = k - l;
    int dirf = lsame_reimpl(direct, 'F');
    int colv = lsame_reimpl(storev, 'C');
    int qr = dirf && colv;
    int lq = dirf && !colv;
    int ql = !dirf && colv;

    if (qr) {
        /* Recursive calls for T11 and T22 */
        hflarft(direct, storev, n, l, v, ldv, tau, t, ldt);
        hflarft(direct, storev, n - l, kl, &v[l + l * ldv], ldv, &tau[l], &t[l + l * ldt], ldt);

        /* T12 = V21' */
        for (int j = 0; j < l; j++) {
            for (int i = 0; i < kl; i++) {
                t[j + (l + i) * ldt] = v[(l + i) + j * ldv];
            }
        }

        /* T12 = T12 * V22 */
        hftrmm('R', 'L', 'N', 'U', l, kl, ONE, &v[l + l * ldv], ldv, &t[l * ldt], ldt);

        /* T12 += V31' * V32 */
        if (n > k) {
            int nk = n - k;
            hfgemm('T', 'N', l, kl, nk, ONE, 
                   &v[k], ldv, 
                   &v[k + l * ldv], ldv, 
                   ONE, &t[l * ldt], ldt);
        }

        /* T12 = -T11 * T12 */
        hftrmm('L', 'U', 'N', 'N', l, kl, -ONE, t, ldt, &t[l * ldt], ldt);

        /* T12 = T12 * T22 */
        hftrmm('R', 'U', 'N', 'N', l, kl, ONE, &t[l + l * ldt], ldt, &t[l * ldt], ldt);

    } else if (lq) {
        /* Recursive calls for T11 and T22 */
        hflarft(direct, storev, n, l, v, ldv, tau, t, ldt);
        hflarft(direct, storev, n - l, kl, &v[l + l * ldv], ldv, &tau[l], &t[l + l * ldt], ldt);

        /* Copy V12 to T12 */
        hflacpy('A', l, kl, &v[l * ldv], ldv, &t[l * ldt], ldt);

        /* T12 = T12 * V22' */
        hftrmm('R', 'U', 'T', 'U', l, kl, ONE, &v[l + l * ldv], ldv, &t[l * ldt], ldt);

        /* T12 += V13 * V23' */
        if (n > k) {
            int nk = n - k;
            hfgemm('N', 'T', l, kl, nk, ONE, 
                   &v[k * ldv], ldv, 
                   &v[l + k * ldv], ldv, 
                   ONE, &t[l * ldt], ldt);
        }

        /* T12 = -T11 * T12 */
        hftrmm('L', 'U', 'N', 'N', l, kl, -ONE, t, ldt, &t[l * ldt], ldt);

        /* T12 = T12 * T22 */
        hftrmm('R', 'U', 'N', 'N', l, kl, ONE, &t[l + l * ldt], ldt, &t[l * ldt], ldt);

    } else if (ql) {
        /* Recursive calls for T11 and T22 */
        hflarft(direct, storev, n - l, kl, v, ldv, tau, t, ldt);
        hflarft(direct, storev, n, l, &v[kl * ldv], ldv, &tau[kl], &t[kl + kl * ldt], ldt);

        /* T21 = V22' */
        for (int j = 0; j < kl; j++) {
            for (int i = 0; i < l; i++) {
                t[(kl + i) + j * ldt] = v[(n - k + j) + (kl + i)* ldv];
            }
        }

        /* T21 = T21 * V21 */
        hftrmm('R', 'U', 'N', 'U', l, kl, ONE, &v[n - k], ldv, &t[kl], ldt);

        /* T21 += V32' * V31 */
        if (n > k) {
            int nk = n - k;
            hfgemm('T', 'N', l, kl, nk, ONE, 
                   &v[kl * ldv], ldv, 
                   v, ldv, 
                   ONE, &t[kl], ldt);
        }

        /* T21 = -T22 * T21 */
        hftrmm('L', 'L', 'N', 'N', l, kl, -ONE, &t[kl + kl * ldt], ldt, &t[kl], ldt);

        /* T21 = T21 * T11 */
        hftrmm('R', 'L', 'N', 'N', l, kl, ONE, t, ldt, &t[kl], ldt);

    } else {
        /* RQ case */
        hflarft(direct, storev, n - l, kl, v, ldv, tau, t, ldt);
        hflarft(direct, storev, n, l, &v[kl], ldv, &tau[kl], &t[kl + kl * ldt], ldt);

        /* Copy V22 to T21 */
        hflacpy('A', l, kl, &v[kl + (n - k) * ldv], ldv, &t[kl], ldt);

        /* T21 = T21 * V12' */
        hftrmm('R', 'L', 'T', 'U', l, kl, ONE, &v[(n - k) * ldv], ldv, &t[kl], ldt);

        /* T21 += V21 * V11' */
        if (n > k) {
            int nk = n - k;
            hfgemm('N', 'T', l, kl, nk, ONE, 
                   &v[kl], ldv, 
                   v, ldv, 
                   ONE, &t[kl], ldt);
        }

        /* T21 = -T22 * T21 */
        hftrmm('L', 'L', 'N', 'N', l, kl, -ONE, &t[kl + kl * ldt], ldt, &t[kl], ldt);

        /* T21 = T21 * T11 */
        hftrmm('R', 'L', 'N', 'N', l, kl, ONE, t, ldt, &t[kl], ldt);
    }
}