
#include "lapacke_utils_reimpl.h"

void hfsteqr(const char compz, int n, lapack_float *d, lapack_float *e, lapack_float *z, int ldz, lapack_float *work, int *info) {
    // Declaracion de constantes
    int MAXIT = 30;
    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;
    const lapack_float TWO = (lapack_float) 2.0;
    const lapack_float THREE = (lapack_float) 3.0;

    // Declarar todas las variables locales al principio
    int icompz, jtot = 0, iscale, i, k;
    int l, l1 = 1, lm1, lend, lendm1, lendp1, lsv, lendsv;
    int m, mm, mm1, nm1, nmaxit;
    lapack_float anorm, eps, eps2, safmin, safmax, ssfmax, ssfmin, tst, rt1, rt2;
    lapack_float p = ZERO, g = ZERO, r = ZERO, c = ZERO, s = ZERO, f = ZERO, b = ZERO;

    // Check COMPZ
    if (lsame_reimpl(compz, 'N')) icompz = 0;
    else if (lsame_reimpl(compz, 'V')) icompz = 1;
    else if (lsame_reimpl(compz, 'I')) icompz = 2;
    else icompz = -1;

    *info = 0;
    if (icompz < 0) *info = -1;
    else if (n < 0) *info = -2;
    else if (ldz < 1 || (icompz > 0 && ldz < MAX(1,n))) *info = -6;
    if (*info != 0) {
        LAPACKE_xerbla("HFSTEQR", -(*info));
        return;
    }

    // Quick return for n=0 or n=1
    if (n == 0) return;
    if (n == 1) {
        if (icompz == 2) z[0] = ONE;
        return;
    }

    // Machine constants
    eps = hflamch_half_precision('E');
    eps2 = eps * eps;
    safmin = hflamch_half_precision('S');
    safmax = ONE / safmin;
    ssfmax = custom_sqrtf_half_precision(safmax) / THREE;
    ssfmin = custom_sqrtf_half_precision(safmin) / eps2;

    // Initialize Z to identity if needed
    if (icompz == 2) hflaset('F', n, n, ZERO, ONE, z, ldz); // 'F' for full matrix

    nmaxit = n * MAXIT;
    jtot = 0;

    l1 = 1;
    nm1 = n - 1;
        
    do{
        while(1){ // 10
            
            if(l1 > n){
                    // 160
                if(icompz == 0){
                    // Use Quick Sort
                    hflasrt('I', n, d, info);
                }else{
                    // Use Selection Sort to minimize swaps of eigenvectors

                    for(int ii = 2; ii <= n; ii++){
                        i = ii - 1;
                        k = i;
                        p = d[i - 1];

                        for(int j = ii; j <= n; j++){
                            if(d[j - 1] < p){
                                k = j;
                                p = d[j - 1];
                            }
                        }
                        if(k != i){
                            d[k - 1] = d[i - 1];
                            d[i - 1] = p;
                            
                            hfswap(n, &z[(i - 1) * ldz], 1, &z[(k - 1) * ldz], 1); // Column-major

                        }
                    }
                }
                return;
                // END 160
            } 
            if(l1 > 1) e[l1-2] = ZERO; // Ajustar índices
            if(l1 <= nm1){
                for(m = l1; m <= nm1; m++){
                    tst = ABS_half_precision(e[m-1]);

                    if(tst == ZERO) break;

                    if(tst <= custom_sqrtf_half_precision(ABS_half_precision(d[m-1])) * custom_sqrtf_half_precision(ABS_half_precision(d[m])) * eps) {
                        e[m-1] = ZERO;
                        break;
                    }
                }
                if(m > nm1){
                    m = n;
                }
            }else{
                m = n;
            }
            
            l = l1;
            lsv = l;
            lend = m;
            lendsv = lend;
            l1 = m + 1;
            if(lend == l) continue;
            
            anorm = hflanst('M', lend - l + 1, &d[l-1], &e[l-1]);
            iscale = 0;
            if(anorm == ZERO) continue;
            break;

        } //////// FIN WHILE 1

        if(anorm > ssfmax) {
            iscale = 1;
            hflascl('G', 0, 0, anorm, ssfmax, lend - l + 1, 1, &d[l-1], n, info);
            hflascl('G', 0, 0, anorm, ssfmax, lend - l, 1, &e[l-1], n, info);
        } else if (anorm < ssfmin) {
            iscale = 2;
            hflascl('G', 0, 0, anorm, ssfmin, lend - l + 1, 1, &d[l-1], n, info);
            hflascl('G', 0, 0, anorm, ssfmin, lend - l, 1, &e[l-1], n, info);
        }

        // Choose between QL and QR iteration

        if(ABS_half_precision(d[lend-1]) < ABS_half_precision(d[l-1])) {
            lend = lsv;
            l = lendsv;
        }

        if(lend > l){
            
            while(1){  // 40    // QL Iteration
                
                if(l != lend){ //  Look for small subdiagonal element.
                    lendm1 = lend - 1;
                    for(m = l; m <= lendm1; m++){
                        tst = ABS_half_precision(e[m-1]) * ABS_half_precision(e[m-1]);
                        if(tst <= eps2 * ABS_half_precision(d[m-1]) * ABS_half_precision(d[m]) + safmin) break;
                    }
                    if(m > lendm1) m = lend;

                }else {
                    m = lend;
                }

                if(m < lend) e[m-1] = ZERO;
                p = d[l-1];
                if(m == l){ // Eigenvalue found
                    // 80
                    d[l-1] = p;
                    l += 1;
                    if(l <= lend) continue;
                    break;
                }

            
                if(m == l + 1){ // The remaining matrix is 2-by-2
                    if(compz > 0){
                        hflaev2(d[l-1], e[l-1], d[l], &rt1, &rt2, &c, &s);
                        work[l-1] = c;
                        work[n - 1 + l - 1] = s;
                        hflasr('R', 'V', 'B', n, 2, &work[l-1], &work[n - 1 + l - 1], &z[(l-1) * ldz], ldz); // Revisar por si se está pasando como row-major
                    }else{
                        hflae2(d[l-1], e[l-1], d[l], &rt1, &rt2);
                    }
                    d[l-1] = rt1;
                    d[l] = rt2;
                    e[l-1] = ZERO;
                    l += 2;
                    if(l <= lend) continue;
                    break;

                }
                if(jtot == nmaxit) break;
                jtot++;

                // Form shift

                g = (d[l] - p) / (TWO * e[l-1]);
                r = hflapy2(g, ONE);
                //g = d[m-1] - p + (e[l-1] / (g + (g > ZERO ? r : -r)));
                g = d[m-1] - p + (e[l-1] / (g + (lapack_float)copysignf((float)r, (float)g)));

                s = ONE;
                c = ONE;
                p = ZERO;

                // Inner loop

                mm1 = m - 1;
                for(i = mm1; i >= l; i--){
                    f = s * e[i - 1];
                    b = c * e[i - 1];
                    hflartg(g, f, &c, &s, &r);
                    if(i != m - 1){
                        e[i] = r;
                    }
                    g = d[i] - p;
                    r = (d[i - 1] - g) * s + TWO * c * b;
                    p = s * r;
                    d[i] = g + p;
                    g = c * r - b;
                    
                    // If eigenvectors are desired, then save rotations.

                    if(icompz > 0){
                        work[i - 1] = c;
                        work[n - 1 + i - 1] = -s;
                    }



                }

                // If eigenvectors are desired, then apply saved rotations
                if(icompz > 0) {
                    mm = m - l + 1;
                    hflasr('R', 'V', 'B', n, mm, &work[l-1], &work[n - 1 + l - 1], &z[(l-1) * ldz], ldz); // Revisar por si se está pasando como row-major
                }

                d[l - 1] = d[l - 1] - p;
                e[l - 1] = g;   
            }

        } else{
            // QR Iteration

            while(1){ // 90

                if(l <= lend){ // Look for small superdiagonal element.
                    lendp1 = lend + 1;
                    for(m = l; m >= lendp1; m--){
                        tst = ABS_half_precision(e[m-2]) * ABS_half_precision(e[m-2]);
                        if(tst<= (eps2 * ABS_half_precision(d[m-1]) * ABS_half_precision(d[m-2]) + safmin)) break;
                    }
                    if(m < lendp1) m = lend;
                } else{
                    m = lend;
                }
                // 110
                if(m < lend) e[m-2] = ZERO;
                p = d[l-1];

                if(m == l){ // Eigenvalue found
                    // 130
                    d[l-1] = p; 
                    l--;
                    if(l >= lend) continue;
                    break;
                }
                if(m == l - 1){  // The remaining matrix is 2-by-2
                    if(compz > 0){
                        hflaev2(d[l-2], e[l-2], d[l-1], &rt1, &rt2, &c, &s);
                        work[m-1] = c;
                        work[n - 1 + m - 1] = s;
                        hflasr('R', 'V', 'F', n, 2, &work[m-1], &work[n - 1 + m - 1], &z[(l-2) * ldz], ldz); // Revisar por si se está pasando como row-major
                    }else{
                        hflae2(d[l-2], e[l-2], d[l-1], &rt1, &rt2);
                    }

                    d[l - 2] = rt1;
                    d[l - 1] = rt2;
                    e[l - 2] = ZERO;
                    l -= 2;
                    if(l >= lend) continue;
                    break;
                }

                if(jtot == nmaxit) break;
                jtot++;

                // Form shift

                g = (d[l-2] - p) / (TWO * e[l-2]);
                r = hflapy2(g, ONE);
                g = d[m-1] - p + (e[l-2] / (g + (lapack_float)copysignf((float)r, (float)g)));

                s = ONE;
                c = ONE;
                p = ZERO;

                // Inner loop

                lm1 = l - 1;

                for(i = m; i <= lm1; i++){
                    f = s * e[i-1];
                    b = c * e[i-1];
                    hflartg(g, f, &c, &s, &r);
                    if(i != m){
                        e[i-2] = r;
                    }
                    g = d[i-1] - p;
                    r = (d[i] - g) * s + TWO * c * b;
                    p = s * r;
                    d[i-1] = g + p;
                    g = c * r - b;
                    // If eigenvectors are desired, then save rotations.
                    if(icompz > 0){
                        work[i-1] = c;
                        work[n - 1 + i - 1] = s;
                    }
                }

                // If eigenvectors are desired, then apply saved rotations.
                if(icompz > 0) {
                    mm = l - m + 1;
                    hflasr('R', 'V', 'F', n, mm, &work[m-1], &work[n - 1 + m - 1], &z[(m - 1) * ldz], ldz); // Revisar por si se está pasando como row-major
                }

                d[l-1] = d[l-1] - p;
                e[lm1-1] = g;
                l--;

            }

        } // 140

        // Undo scaling if necessary

        if(iscale == 1){
            hflascl('G', 0, 0, ssfmax, anorm, lendsv - lsv + 1, 1, &d[lsv - 1], n, info);
            hflascl('G', 0, 0, ssfmax, anorm, lendsv - lsv, 1, &e[lsv - 1], n, info);
        }else if(iscale == 2){
            hflascl('G', 0, 0, ssfmin, anorm, lendsv - lsv + 1, 1, &d[lsv - 1], n, info);
            hflascl('G', 0, 0, ssfmin, anorm, lendsv - lsv, 1, &e[lsv - 1], n, info);
        }

        if(jtot >= nmaxit){
            // GO TO 10
            break;
        }

    }while(1);

    for(i = 1; i <= n - 1; i++){
        if(e[i-1] != ZERO){
            info++;
        }
    }
    // 190
    return;
}
