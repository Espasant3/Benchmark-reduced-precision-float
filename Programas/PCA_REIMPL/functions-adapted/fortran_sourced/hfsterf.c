
#include "lapacke_utils_reimpl.h"

void hfsterf(lapack_int n, lapack_float *d, lapack_float *e, int *info) {
    // Declaración de constantes
    int MAXIT = 30;
    lapack_float ZERO = (lapack_float) 0.0;
    lapack_float ONE = (lapack_float) 1.0;
    lapack_float TWO = (lapack_float) 2.0;
    lapack_float THREE = (lapack_float) 3.0;

    int i, iscale, jtot, l, l1, lend, lendsv, lsv, m, nmaxit;
    lapack_float alpha, anorm, bb, c, eps, eps2, gamma, oldc, oldgam, p, r;
    lapack_float rt1, rt2, rte, s, safmax, safmin, sigma, ssfmax, ssfmin;

    int condition_for = 0;

    *info = 0;
    if (n < 0) {
        *info = -1;
        LAPACKE_xerbla("HFSTERF", -(*info));
        return;
    }
    if (n <= 1) return;

    // Determine the unit roundoff for this environment.

    eps = hflamch_half_precision('E');
    eps2 = eps * eps;
    safmin = hflamch_half_precision('S');
    safmax = ONE / safmin;
    ssfmax = custom_sqrtf_half_precision(safmax) / THREE;
    ssfmin = custom_sqrtf_half_precision(safmin) / eps2;

    nmaxit = n * MAXIT;
    sigma = ZERO;
    jtot = 0;
    l1 = 1;

    do { // 10
        if (l1 > n){
            hflasrt('I', n, d, info);
            return;
        }
        if (l1 > 1) e[l1-2] = ZERO;

        for (m = l1 - 1; m < n-1; m++) {
            if (ABS_half_precision(e[m]) <= (custom_sqrtf_half_precision(ABS_half_precision(d[m])) * sqrtf(ABS_half_precision(d[m+1]))) * eps) {
                e[m] = ZERO;
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

        for (i = l-1; i < lend-1; i++)
            e[i] *= e[i];

        if (ABS_half_precision(d[lend-1]) < ABS_half_precision(d[l-1])) {
            lend = lsv;
            l = lendsv;
        }

        if (lend >= l) { 
            
            // QL Iteration

            while (1) { // 50

                if (l != lend){
                    for (m = l - 1; m < lend - 1; m++) {
                        if (ABS_half_precision(e[m]) <= eps2 * ABS_half_precision(d[m] * d[m+1])){
                            condition_for = 1;
                            break;
                        }
                    }
                }
                if (!condition_for) {
                    m = lend - 1;
                }

                condition_for = 0;
               
                if (m < lend) e[m-1] = ZERO;
                p = d[l-1];
                if (m == l){
                    // Go to 90
                    d[l-1] = p;
                    l++;
                    if(l <= lend) continue;
                    break;
                }

                if (m == l+1) {
                    rte = custom_sqrtf_half_precision(e[l-1]);
                    hflae2(d[l-1], rte, d[l], &rt1, &rt2);
                    d[l-1] = rt1;
                    d[l] = rt2;
                    e[l-1] = ZERO;
                    l += 2;
                    if(l <= lend) {
                        continue;
                    }
                    break;
                }


                if (jtot == nmaxit) break;
                jtot++;

                // Form shift

                rte = custom_sqrtf_half_precision(e[l-1]);
                sigma = (d[l] - p) / (TWO * rte);
                r = hflapy2(sigma, ONE);
                sigma = p - (rte / (sigma + (lapack_float) copysignf((float)r, (float)sigma)));

                c = ONE;
                s = ZERO;
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
                    if(c != ZERO){
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
                    if (ABS_half_precision(e[m-1]) <= eps2 * ABS_half_precision(d[m] * d[m-1])) {
                        condition_for = 1;
                        break;
                    }
                }
                if(!condition_for) {
                    m = lend;
                }
                condition_for = 0;

                if (m > lend){
                    e[m-2] = ZERO;   
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
                    rte = custom_sqrtf_half_precision(e[l-2]);
                    hflae2(d[l-1], rte, d[l-2], &rt1, &rt2);
                    d[l-1] = rt1;
                    d[l-2] = rt2;
                    e[l-2] = ZERO;
                    l -= 2;
                    if(l >= lend){
                        continue;
                    }
                    break;
                }

                if (jtot == nmaxit) break;
                jtot++;

                // Form shift

                rte = custom_sqrtf_half_precision(e[l-2]);
                sigma = (d[l-2] - p) / (TWO * rte);
                r = hflapy2(sigma, ONE);
                sigma = p - (rte / (sigma + (lapack_float) copysignf((float)r, (float)sigma)));

                c = ONE;
                s = ZERO;
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
                    if(c != ZERO){
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
        if (e[i] != ZERO) {
            *info += 1;
        }
    }
    return;
}