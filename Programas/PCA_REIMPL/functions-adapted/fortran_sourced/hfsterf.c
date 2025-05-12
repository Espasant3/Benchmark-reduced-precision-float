
#include "../include/lapacke_utils_reimpl.h"

/* Constantes */
#define ZERO 0.0F16
#define ONE 1.0F16
#define TWO 2.0F16
#define THREE 3.0F16
#define MAXIT 30

void hfsterf(lapack_int n, _Float16 *d, _Float16 *e, int *info) {
    int i, iscale, jtot, l, l1, lend, lendsv, lsv, m, nmaxit;
    _Float16 alpha, anorm, bb, c, eps, eps2, gamma, oldc, oldgam, p, r;
    _Float16 rt1, rt2, rte, s, safmax, safmin, sigma, ssfmax, ssfmin;

    *info = 0;
    if (n < 0) {
        *info = -1;
        LAPACKE_xerbla("HFSTERF", -(*info));
        return;
    }
    if (n <= 1) return;

    eps = hflamch('E');
    eps2 = eps * eps;
    safmin = hflamch('S');
    safmax = ONE / safmin;
    ssfmax = sqrtf(safmax) / THREE;
    ssfmin = sqrtf(safmin) / eps2;

    nmaxit = n * MAXIT;
    sigma = ZERO;
    jtot = 0;
    l1 = 1;

    while (1) {
        if (l1 > n) break;
        if (l1 > 1) e[l1-2] = ZERO;

        for (m = l1; m <= n-1; ++m) {
            if (ABS_Float16(e[m-1]) <= sqrtf(ABS_Float16(d[m-1])) * sqrtf(ABS_Float16(d[m])) * eps) {
                e[m-1] = ZERO;
                break;
            }
        }
        m = (m > n-1) ? n : m;

        l = l1;
        lsv = l;
        lend = m;
        lendsv = lend;
        l1 = m + 1;
        if (lend == l) continue;

        anorm = hflanst('M', lend-l+1, &d[l-1], &e[l-1]);
        iscale = 0;
        if (anorm == ZERO) continue;

        if (anorm > ssfmax) {
            iscale = 1;
            hflascl('G', 0, 0, anorm, ssfmax, lend-l+1, 1, &d[l-1], n, info);
            hflascl('G', 0, 0, anorm, ssfmax, lend-l, 1, &e[l-1], n, info);
        } else if (anorm < ssfmin) {
            iscale = 2;
            hflascl('G', 0, 0, anorm, ssfmin, lend-l+1, 1, &d[l-1], n, info);
            hflascl('G', 0, 0, anorm, ssfmin, lend-l, 1, &e[l-1], n, info);
        }

        for (i = l-1; i < lend-1; ++i)
            e[i] = e[i] * e[i];

        if (ABS_Float16(d[lend-1]) < ABS_Float16(d[l-1])) {
            lend = lsv;
            l = lendsv;
        }

        if (lend >= l) {
            // QL Iteration
            while (1) {
                if (l == lend) break;
                for (m = l; m < lend; ++m) {
                    if (ABS_Float16(e[m-1]) <= eps2 * ABS_Float16(d[m-1] * d[m]))
                        break;
                }
                if (m < lend) e[m-1] = ZERO;
                p = d[l-1];
                if (m == l) break;

                if (m == l+1) {
                    rte = sqrtf(e[l-1]);
                    hflae2(d[l-1], rte, d[l], &rt1, &rt2);
                    d[l-1] = rt1;
                    d[l] = rt2;
                    e[l-1] = ZERO;
                    l += 2;
                    continue;
                }

                if (jtot == nmaxit) break;
                jtot++;

                rte = sqrtf(e[l-1]);
                sigma = (d[l] - p) / (TWO * rte);
                r = hypotf(sigma, ONE);
                sigma = p - (rte / (sigma + (_Float16) copysignf((float)r, (float)sigma)));

                c = ONE;
                s = ZERO;
                gamma = d[m-1] - sigma;
                p = gamma * gamma;

                for (i = m-1; i >= l; --i) {
                    bb = e[i-1];
                    r = p + bb;
                    if (i != m-1) e[i] = s * r;
                    oldc = c;
                    c = p / r;
                    s = bb / r;
                    oldgam = gamma;
                    alpha = d[i-1];
                    gamma = c * (alpha - sigma) - s * oldgam;
                    d[i] = oldgam + (alpha - gamma);
                    p = (c != ZERO) ? (gamma * gamma) / c : oldc * bb;
                }

                e[l-1] = s * p;
                d[l-1] = sigma + gamma;
            }
        } else {
            // QR Iteration
            while (1) {
                if (l == lend + 1) break;
                for (m = l; m > lend; --m) {
                    if (ABS_Float16(e[m-2]) <= eps2 * ABS_Float16(d[m-1] * d[m-2]))
                        break;
                }
                if (m > lend) e[m-2] = ZERO;
                p = d[l-1];
                if (m == l) break;

                if (m == l-1) {
                    rte = sqrtf(e[l-2]);
                    hflae2(d[l-1], rte, d[l-2], &rt1, &rt2);
                    d[l-1] = rt1;
                    d[l-2] = rt2;
                    e[l-2] = ZERO;
                    l -= 2;
                    continue;
                }

                if (jtot == nmaxit) break;
                jtot++;

                rte = sqrtf(e[l-2]);
                sigma = (d[l-2] - p) / (TWO * rte);
                r = hypotf(sigma, ONE);
                sigma = p - (rte / (sigma + (_Float16) copysignf((float)r, (float)sigma)));

                c = ONE;
                s = ZERO;
                gamma = d[m-1] - sigma;
                p = gamma * gamma;

                for (i = m-1; i < l-1; ++i) {
                    bb = e[i-1];
                    r = p + bb;
                    if (i != m-1) e[i-2] = s * r;
                    oldc = c;
                    c = p / r;
                    s = bb / r;
                    oldgam = gamma;
                    alpha = d[i];
                    gamma = c * (alpha - sigma) - s * oldgam;
                    d[i-1] = oldgam + (alpha - gamma);
                    p = (c != ZERO) ? (gamma * gamma) / c : oldc * bb;
                }

                e[l-2] = s * p;
                d[l-1] = sigma + gamma;
            }
        }

        // Restablecer escalado
        if (iscale == 1)
            hflascl('G', 0, 0, ssfmax, anorm, lendsv-lsv+1, 1, &d[lsv-1], n, info);
        if (iscale == 2)
            hflascl('G', 0, 0, ssfmin, anorm, lendsv-lsv+1, 1, &d[lsv-1], n, info);
    }

    // Verificar convergencia
    if (jtot < nmaxit) {
        for (i = 0; i < n-1; ++i)
            if (e[i] != ZERO) (*info)++;
    }

    // Ordenar eigenvalores
    hflasrt('I', n, d, info);
}