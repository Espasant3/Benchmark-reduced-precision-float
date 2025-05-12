
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
    printf("LAPACKE_hfsteqr: n = %d, ldz = %d\n", n, ldz);
    // Declaracion de constantes
    int MAXIT = 30;

    // Declarar todas las variables locales al principio
    int icompz, l1 = 1, lend, lsv, lendsv, m, nm1, jtot = 0, nmaxit;
    _Float16 anorm, eps, eps2, safmin, safmax, ssfmax, ssfmin, tst;
    _Float16 p = 0.0F16, g = 0.0F16, r = 0.0F16, c = 0.0F16, s = 0.0F16, f = 0.0F16, b = 0.0F16;

    // Check COMPZ
    if (lsame_reimpl(compz, 'N')) icompz = 0;
    else if (lsame_reimpl(compz, 'V')) icompz = 1;
    else if (lsame_reimpl(compz, 'I')) icompz = 2;
    else icompz = -1;

    *info = 0;
    if (icompz < 0) *info = -1;
    else if (n < 0) *info = -2;
    else if (ldz < 1 || (icompz > 0 && ldz < n)) *info = -6;
    if (*info != 0) {
        LAPACKE_xerbla("HFSTEQR", *info);
        return;
    }

    // Quick return for n=0 or n=1
    if (n == 0) return;
    if (n == 1) {
        if (icompz == 2) z[0] = 1.0F16;
        return;
    }

    // Machine constants
    eps = hflamch('E');
    eps2 = eps * eps;
    safmin = hflamch('S');
    safmax = 1.0F16 / safmin;
    ssfmax = custom_sqrtf16(safmax) / 1.0F16;
    ssfmin = custom_sqrtf16(safmin) / eps2;

    // Initialize Z to identity if needed
    if (icompz == 2) hflaset('A', n, n, 0.0F16, 1.0F16, z, ldz);

    nmaxit = n * MAXIT;
    nm1 = n - 1;

    printf("Entrando en el bucle principal\n");
    printf("l1 = %d, n = %d\n", l1, n);
    printf("nm1 = %d\n", nm1);
    printf("nmaxit = %d\n", nmaxit);
    while (l1 <= n) {

        if (l1 > 1) e[l1-2] = 0.0F16; // Adjust indices

        // Find m (split point)
        for (m = l1-1; m < nm1; m++) {
            tst = ABS_Float16(e[m]);
            if (tst <= custom_sqrtf16(ABS_Float16(d[m])) * custom_sqrtf16(ABS_Float16(d[m+1])) * eps) {
                e[m] = 0.0F16;
                break;
            }
        }
        //if (m == nm1) m = n;
        if (m == nm1) {
            l1 = n + 1;  // Salir del bucle en la siguiente iteración
        } else {
            l1 = m + 1;  // Continuar procesamiento normal
        }

        int l = l1 - 1;
        lsv = l;
        lend = m;
        lendsv = lend;
        //l1 = m + 1;
        if (lend == l) continue;

        // Scale submatrix
        int len = lend - l + 1;
        anorm = hflanst('M', len, &d[l], &e[l]);
        int iscale = 0;
        if (anorm == 0.0F16) continue;
        
        // Corregido (usar variables temporales):
        int kl = 0, ku = 0, one = 1;

        if (anorm > ssfmax) {
            iscale = 1;
            hflascl('G', kl, ku, anorm, ssfmax, len, one, &d[l], n, info);
            hflascl('G', kl, ku, anorm, ssfmax, len - 1, one, &e[l], n, info);
        } else if (anorm < ssfmin) {
            iscale = 2;
            hflascl('G', kl, ku, anorm, ssfmin, len, one, &d[l], n, info);
            hflascl('G', kl, ku, anorm, ssfmin, len - 1, one, &e[l], n, info);
        }

        // QL/QR iteration
        if (ABS_Float16(d[lend-1]) < ABS_Float16(d[l-1])) {
            lend = lsv;
            l = lendsv;
        }

        if (lend > l) {
            // QL Iteration
            c = 1.0F16;  // Valor inicial para rotaciones
            s = 1.0F16;
            p = 0.0F16;
            int lendm1 = lend - 1;
            while (1) {
                // Buscar elemento subdiagonal pequeño (índices 0-based)
                int m = lend;
                if (l != lend) {
                    for (m = l; m < lendm1; m++) {
                        tst = ABS_Float16(e[m]) * ABS_Float16(e[m]);
                        if (tst <= (eps2 * ABS_Float16(d[m])) * (ABS_Float16(d[m+1]) + safmin)) 
                            break;
                    }
                }
        
                if (m < lend) e[m] = 0.0F16;
                p = d[l];
                if (m == l) break;
        
                // Caso 2x2 (ajustar índices)
                if (m == l + 1) {
                    _Float16 rt1, rt2;
                    if (icompz > 0) {
                        hflaev2(d[l], e[l], d[l+1], &rt1, &rt2, &c, &s);
                        work[l] = c;
                        work[n - 1 + l] = s;
                        int two = 2;
                        hflasr('R', 'V', 'B', n, two, &work[l], &work[n - 1 + l], 
                               &z[l * ldz], ldz);
                    } else {
                        hflae2(d[l], e[l], d[l+1], &rt1, &rt2);
                    }
                    d[l] = rt1;
                    d[l+1] = rt2;
                    e[l] = 0.0F16;
                    l += 2;
                    if (l <= lend) continue;
                    break;
                }
        
                if (jtot == nmaxit) break;
                jtot++;
        
                // Cálculo del shift (0-based)
                g = (d[l+1] - p) / (2.0F16 * e[l]);
                r = hflapy2(g, 1.0F16);
                g = d[m] - p + (e[l] / (g + (g > 0 ? r : -r)));
                
        
                // Inner loop (rotaciones hacia atrás)
                c = 1.0F16;
                s = 1.0F16;
                p = 0.0F16;
                for (int i = m - 1; i >= l; i--) {
                    f = s * e[i];
                    b = c * e[i];
                    hflartg(g, f, &c, &s, &r);
                    if (i != m - 1) e[i+1] = r;
                    g = d[i+1] - p;
                    r = (d[i] - g) * s + 2.0F16 * c * b;
                    p = s * r;
                    d[i+1] = g + p;
                    g = c * r - b;
        
                    // Guardar rotaciones en WORK
                    if (icompz > 0) {
                        work[i] = c;
                        work[n - 1 + i] = -s;
                    }
                }
        
                // Aplicar rotaciones a Z (0-based)
                if (icompz > 0) {
                    int mm = m - l;
                    hflasr('R', 'V', 'B', n, mm, &work[l], &work[n - 1 + l], 
                           &z[l * ldz], ldz);
                }
        
                d[l] -= p;
                e[l] = g;
            }
        } else {
            // QR Iteration
            c = 1.0F16;  // Valor inicial para rotaciones
            s = 1.0F16;
            p = 0.0F16;
            while (1) {
                // Buscar elemento superdiagonal pequeño (0-based)
                int m = lend;
                if (l != lend) {
                    for (m = l; m > lend; m--) {
                        tst = ABS_Float16(e[m-1]) * ABS_Float16(e[m-1]);
                        if (tst <= (eps2 * ABS_Float16(d[m])) * (ABS_Float16(d[m-1]) + safmin)) 
                            break;
                    }
                }
        
                if (m > lend) e[m-1] = 0.0F16;
                p = d[l];
                if (m == l) break;
        
                // Caso 2x2 (0-based)
                if (m == l - 1) {
                    _Float16 rt1, rt2;
                    if (icompz > 0) {
                        hflaev2(d[l-1], e[l-1], d[l], &rt1, &rt2, &c, &s);
                        work[m] = c;
                        work[n - 1 + m] = s;
                        int two = 2;
                        hflasr('R', 'V', 'F', n, two, &work[m], &work[n - 1 + m], 
                               &z[(l-1) * ldz], ldz);
                    } else {
                        hflae2(d[l-1], e[l-1], d[l], &rt1, &rt2);
                    }
                    d[l-1] = rt1;
                    d[l] = rt2;
                    e[l-1] = 0.0F16;
                    l -= 2;
                    if (l >= lend) continue;
                    break;
                }
        
                if (jtot == nmaxit) break;
                jtot++;
        
                // Cálculo del shift
                g = (d[l-1] - p) / (2.0F16 * e[l-1]);
                r = hflapy2(g, 1.0F16);
                g = d[m] - p + (e[l-1] / (g + (g > 0 ? r : -r)));
        
                // Inner loop (rotaciones hacia adelante)
                c = 1.0F16;
                s = 1.0F16;
                p = 0.0F16;
                for (int i = m; i < l; i++) {
                    f = s * e[i];
                    b = c * e[i];
                    hflartg(g, f, &c, &s, &r);
                    if (i != m) e[i-1] = r;
                    g = d[i] - p;
                    r = (d[i+1] - g) * s + 2.0F16 * c * b;
                    p = s * r;
                    d[i] = g + p;
                    g = c * r - b;
        
                    // Guardar rotaciones en WORK
                    if (icompz > 0) {
                        work[i] = c;
                        work[n - 1 + i] = s;
                    }
                }
        
                // Aplicar rotaciones a Z
                if (icompz > 0) {
                    int mm = l - m;
                    hflasr('R', 'V', 'F', n, mm, &work[m], &work[n - 1 + m], 
                           &z[m * ldz], ldz);
                }
        
                d[l] -= p;
                e[l-1] = g;
            }
        }

        // Undo scaling
        if (iscale == 1) {
            hflascl('G', kl, ku, ssfmax, anorm, lendsv - lsv + 1, one, &d[lsv], n, info);
            hflascl('G', kl, ku, ssfmax, anorm, lendsv - lsv, one, &e[lsv], n, info);
        } else if (iscale == 2) {
            hflascl('G', kl, ku, ssfmin, anorm, lendsv - lsv + 1, one, &d[lsv], n, info);
            hflascl('G', kl, ku, ssfmin, anorm, lendsv - lsv, one, &e[lsv], n, info);
        }
    }

    // Check convergence (0-based)
    if (jtot < nmaxit) {
        l1 = 1; // Resetear para nueva iteración
    } else {
        // Contar elementos no convergentes en E
        *info = 0;
        for (int i = 0; i < n - 1; i++) {
            if (e[i] != 0.0F16) (*info)++;
            // Esto se podría modificar para que fuera ABS_Float16(e[i]) > eps // (epsilon)
        }
    }

    // Sort eigenvalues and vectors
    if (icompz == 0) {
        hflasrt('I', n, d, info);
    } else {
        // Selection sort (adjusted for 0-based)
        for (int i = 0; i < n - 1; i++) {
            int k = i;
            _Float16 p_val = d[i];
            for (int j = i+1; j < n; j++) {
                if (d[j] < p_val) {
                    k = j;
                    p_val = d[j];
                }
            }
            if (k != i) {
                d[k] = d[i];
                d[i] = p_val;
                //hfswap_row_major(n, &z[i], 1, &z[k], 1); // Acceso por filas (row-major)
                hfswap_columns(n, z, ldz, i, k); // Intercambia columnas i y k
            }
        }
    }
}