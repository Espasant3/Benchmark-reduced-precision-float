
#include "../include/lapacke_utils_reimpl.h"

/* Constantes */
#define MAXIT 30

void hfsterf(lapack_int n, _Float16 *d, _Float16 *e, int *info) {
    int i, iscale, jtot, l, l1, lend, lendsv, lsv, m, nmaxit;
    _Float16 alpha, anorm, bb, c, eps, eps2, gamma, oldc, oldgam, p, r;
    _Float16 rt1, rt2, rte, s, safmax, safmin, sigma, ssfmax, ssfmin;

    int condition_for = 0;

    *info = 0;
    if (n < 0) {
        *info = -1;
        LAPACKE_xerbla("HFSTERF", -(*info));
        return;
    }
    if (n <= 1) return;

    // Determine the unit roundoff for this environment.

    eps = hflamch_Float16('E');
    eps2 = eps * eps;
    safmin = hflamch_Float16('S');
    safmax = 1.0F16 / safmin;
    ssfmax = custom_sqrtf16(safmax) / 3.0F16;
    ssfmin = custom_sqrtf16(safmin) / eps2;

    nmaxit = n * MAXIT;
    sigma = 0.0F16;
    jtot = 0;
    l1 = 1;

    do { // 10
        if (l1 > n){
            hflasrt('I', n, d, info);
            return;
        }
        if (l1 > 1) e[l1-2] = 0.0F16;

        for (m = l1 - 1; m < n-1; m++) {
            if (ABS_Float16(e[m]) <= (custom_sqrtf16(ABS_Float16(d[m])) * sqrtf(ABS_Float16(d[m+1]))) * eps) {
                e[m] = 0.0F16;
                condition_for = 1;
                break;
            }
        }
        if(!condition_for) {
            m = n - 1;
        }
        condition_for = 0;

        l = l1;
        lsv = l;
        lend = m;
        lendsv = lend;
        l1 = m + 1;
        if (lend == l) continue;

        anorm = hflanst('M', lend-l+1, &d[l-1], &e[l-1]);
        iscale = 0;
        if (anorm == 0.0F16) continue;

        if (anorm > ssfmax) {
            iscale = 1;
            hflascl('G', 0, 0, anorm, ssfmax, lend-l+1, 1, &d[l-1], n, info);
            hflascl('G', 0, 0, anorm, ssfmax, lend-l, 1, &e[l-1], n, info);
        } else if (anorm < ssfmin) {
            iscale = 2;
            hflascl('G', 0, 0, anorm, ssfmin, lend-l+1, 1, &d[l-1], n, info);
            hflascl('G', 0, 0, anorm, ssfmin, lend-l, 1, &e[l-1], n, info);
        }

        for (i = l-1; i < lend-1; i++)
            e[i] *= e[i];

        if (ABS_Float16(d[lend-1]) < ABS_Float16(d[l-1])) {
            lend = lsv;
            l = lendsv;
        }

        if (lend >= l) { 
            
            // QL Iteration

            while (1) { // 50

                if (l != lend){
                    for (m = l - 1; m < lend - 1; m++) {
                        if (ABS_Float16(e[m]) <= eps2 * ABS_Float16(d[m] * d[m+1])){
                            condition_for = 1;
                            break;
                        }
                    }
                }
                if (!condition_for) {
                    m = lend - 1;
                }

                condition_for = 0;
               
                if (m < lend) e[m-1] = 0.0F16;
                p = d[l-1];
                if (m == l){
                    // Go to 90
                    d[l-1] = p;
                    l++;
                    if(l <= lend) continue;
                    break;
                }

                if (m == l+1) {
                    rte = custom_sqrtf16(e[l-1]);
                    hflae2(d[l-1], rte, d[l], &rt1, &rt2);
                    d[l-1] = rt1;
                    d[l] = rt2;
                    e[l-1] = 0.0F16;
                    l += 2;
                    if(l <= lend) {
                        continue;
                    }
                    break;
                }


                if (jtot == nmaxit) break;
                jtot++;

                // Form shift

                rte = custom_sqrtf16(e[l-1]);
                sigma = (d[l] - p) / (2.0F16 * rte);
                r = hflapy2(sigma, 1.0F16);
                sigma = p - (rte / (sigma + (_Float16) copysignf((float)r, (float)sigma)));

                c = 1.0F16;
                s = 0.0F16;
                gamma = d[m-1] - sigma;
                p = gamma * gamma;

                // Innner loop

                for (i = m-2; i >= l - 1; i--) {
                    bb = e[i];
                    r = p + bb;
                    if (i + 1 != m - 1) e[i+1] = s * r;
                    oldc = c;
                    c = p / r;
                    s = bb / r;
                    oldgam = gamma;
                    alpha = d[i];
                    gamma = c * (alpha - sigma) - s * oldgam;
                    d[i+1] = oldgam + (alpha - gamma);
                    if(c != 0.0F16){
                        p = (gamma * gamma) / c;
                    } else{
                        p = oldc * bb;
                    }
                }

                e[l-1] = s * p;
                d[l-1] = sigma + gamma;
            }
        } else {
            // QR Iteration
            while (1) {

                for(m = l-1; m > lend + 1; m--) {
                    if (ABS_Float16(e[m-1]) <= eps2 * ABS_Float16(d[m] * d[m-1])) {
                        condition_for = 1;
                        break;
                    }
                }
                if(!condition_for) {
                    m = lend;
                }
                condition_for = 0;

                if (m > lend){
                    e[m-2] = 0.0F16;   
                }

                p = d[l-1];
                
                if( m == l){
                    // Go to 140
                    d[l-1] = p;
                    l--;
                    if (l >= m) continue;
                    break;
                }

                // If remaining matrix is 2 by 2

                if (m == l-1) {
                    rte = custom_sqrtf16(e[l-2]);
                    hflae2(d[l-1], rte, d[l-2], &rt1, &rt2);
                    d[l-1] = rt1;
                    d[l-2] = rt2;
                    e[l-2] = 0.0F16;
                    l -= 2;
                    if(l >= lend){
                        continue;
                    }
                    break;
                }

                if (jtot == nmaxit) break;
                jtot++;

                // Form shift

                rte = custom_sqrtf16(e[l-2]);
                sigma = (d[l-2] - p) / (2.0F16 * rte);
                r = hflapy2(sigma, 1.0F16);
                sigma = p - (rte / (sigma + (_Float16) copysignf((float)r, (float)sigma)));

                c = 1.0F16;
                s = 0.0F16;
                gamma = d[m-1] - sigma;
                p = gamma * gamma;

                for (i = m-1; i < l-1; i++) {
                    bb = e[i-1];
                    r = p + bb;
                    if (i+1 != m) e[i-1] = s * r;
                    oldc = c;
                    c = p / r;
                    s = bb / r;
                    oldgam = gamma;
                    alpha = d[i+1];
                    gamma = c * (alpha - sigma) - s * oldgam;
                    d[i] = oldgam + (alpha - gamma);
                    if(c != 0.0F16){
                        p = (gamma * gamma) / c;
                    } else{
                        p = oldc * bb;
                    }
                }
                e[l-2] = s * p;
                d[l-1] = sigma + gamma;
            }
        }

        // 150
        // Restablecer escalado
        if (iscale == 1)
            hflascl('G', 0, 0, ssfmax, anorm, lendsv-lsv+1, 1, &d[lsv-1], n, info);
        if (iscale == 2)
            hflascl('G', 0, 0, ssfmin, anorm, lendsv-lsv+1, 1, &d[lsv-1], n, info);

        // Check for no convergence

        if(jtot < nmaxit) {
            continue;
        }
        break;
    } while(1);
    for(int i = 0; i < n-1; i++) {
        if (e[i] != 0.0F16) {
            *info += 1;
        }
    }
    return;
}