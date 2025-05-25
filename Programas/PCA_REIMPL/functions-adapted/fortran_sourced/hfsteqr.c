
#include "../include/lapacke_utils_reimpl.h"

void hfswap_columns(int n, _Float16 *z, int ldz, int col1, int col2) {
    for (int row = 0; row < n; row++) {
        // Acceso row-major: fila "row", columnas "col1" y "col2"
        _Float16 temp = z[row * ldz + col1];
        z[row * ldz + col1] = z[row * ldz + col2];
        z[row * ldz + col2] = temp;
    }
}


void hfsteqr(const char compz, int n, _Float16 *d, _Float16 *e, _Float16 *z, int ldz, _Float16 *work, int *info) {
    // Declaracion de constantes
    int MAXIT = 30;

    // Declarar todas las variables locales al principio
    int icompz, jtot = 0, iscale, i, k;
    int l, l1 = 1, lm1, lend, lendm1, lendp1, lsv, lendsv;
    int m, mm, mm1, nm1, nmaxit;
    _Float16 anorm, eps, eps2, safmin, safmax, ssfmax, ssfmin, tst, rt1, rt2;
    _Float16 p = 0.0F16, g = 0.0F16, r = 0.0F16, c = 0.0F16, s = 0.0F16, f = 0.0F16, b = 0.0F16;

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
        if (icompz == 2) z[0] = 1.0F16;
        return;
    }

    // Machine constants
    eps = hflamch_Float16('E');
    eps2 = eps * eps;
    safmin = hflamch_Float16('S');
    safmax = 1.0F16 / safmin;
    ssfmax = custom_sqrtf16(safmax) / 3.0F16;
    ssfmin = custom_sqrtf16(safmin) / eps2;

    // Initialize Z to identity if needed
    if (icompz == 2) hflaset('F', n, n, 0.0F16, 1.0F16, z, ldz); // 'F' for full matrix

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
                            //hfswap(n, &z[i - 1], n, &z[k - 1], n); // Row-major

                        }
                    }
                }
                return;
                // END 160
            } 
            if(l1 > 1) e[l1-2] = 0.0F16; // Ajustar índices
            if(l1 <= nm1){
                for(m = l1; m <= nm1; m++){
                    tst = ABS_Float16(e[m-1]);

                    if(tst == 0.0F16) break;

                    if(tst <= custom_sqrtf16(ABS_Float16(d[m-1])) * custom_sqrtf16(ABS_Float16(d[m])) * eps) {
                        e[m-1] = 0.0F16;
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
            if(anorm == 0.0F16) continue;
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

        if(ABS_Float16(d[lend-1]) < ABS_Float16(d[l-1])) {
            lend = lsv;
            l = lendsv;
        }

        if(lend > l){
            
            while(1){  // 40    // QL Iteration
                
                if(l != lend){ //  Look for small subdiagonal element.
                    lendm1 = lend - 1;
                    for(m = l; m <= lendm1; m++){
                        tst = ABS_Float16(e[m-1]) * ABS_Float16(e[m-1]);
                        if(tst <= eps2 * ABS_Float16(d[m-1]) * ABS_Float16(d[m]) + safmin) break;
                    }
                    if(m > lendm1) m = lend;

                }else {
                    m = lend;
                }

                if(m < lend) e[m-1] = 0.0F16;
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
                    e[l-1] = 0.0F16;
                    l += 2;
                    if(l <= lend) continue;
                    break;

                }
                if(jtot == nmaxit) break;
                jtot++;

                // Form shift

                g = (d[l] - p) / (2.0F16 * e[l-1]);
                r = hflapy2(g, 1.0F16);
                //g = d[m-1] - p + (e[l-1] / (g + (g > 0.0F16 ? r : -r)));
                g = d[m-1] - p + (e[l-1] / (g + (_Float16)copysignf((float)r, (float)g)));

                s = 1.0F16;
                c = 1.0F16;
                p = 0.0F16;

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
                    r = (d[i - 1] - g) * s + 2.0F16 * c * b;
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
                        tst = ABS_Float16(e[m-2]) * ABS_Float16(e[m-2]);
                        if(tst<= (eps2 * ABS_Float16(d[m-1]) * ABS_Float16(d[m-2]) + safmin)) break;
                    }
                    if(m < lendp1) m = lend;
                } else{
                    m = lend;
                }
                // 110
                if(m < lend) e[m-2] = 0.0F16;
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
                    e[l - 2] = 0.0F16;
                    l -= 2;
                    if(l >= lend) continue;
                    break;
                }

                if(jtot == nmaxit) break;
                jtot++;

                // Form shift

                g = (d[l-2] - p) / (2.0F16 * e[l-2]);
                r = hflapy2(g, 1.0F16);
                g = d[m-1] - p + (e[l-2] / (g + (_Float16)copysignf((float)r, (float)g)));

                s = 1.0F16;
                c = 1.0F16;
                p = 0.0F16;

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
                    r = (d[i] - g) * s + 2.0F16 * c * b;
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
        if(e[i-1] != 0.0F16){
            info++;
        }
    }
    // 190
    return;
}
