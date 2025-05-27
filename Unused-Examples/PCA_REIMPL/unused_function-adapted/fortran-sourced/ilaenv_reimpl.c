
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file ilaenv_reimpl.c
 * \brief Implementación de ilaenv en lenguaje C para obtener parámetros de entorno LAPACK
 */

/**
 * \brief Obtiene parámetros de entorno para algoritmos LAPACK (versión reimplementada)
 * 
 * \details Función inspirada en ilaenv de LAPACK pero con:
 *          - Soporte mejorado para codificaciones de caracteres (ASCII/EBCDIC/Prime)
 *          - Lógica extendida para nuevos casos (ej. TR-SYL)
 *          - Compatibilidad con implementaciones modernas de precisión mixta
 * 
 * \param[in] ispec Tipo de parámetro a consultar:
 *                  - 1: Tamaño óptimo de bloque
 *                  - 2: Tamaño mínimo de bloque
 *                  - 3: Punto de corte para algoritmos recursivos
 *                  - 4: Número de shifts (obsoleto)
 *                  - 5: Mínimo de columnas por bloque
 *                  - 6: Crossover para SVD
 *                  - 7: Número de núcleos CPU
 *                  - 8: Crossover multishift QR
 *                  - 9: Tamaño máximo subproblemas
 *                  - 10: Verificación IEEE NaN
 *                  - 11: Verificación de infinitos
 * \param[in] name Nombre de la rutina LAPACK (6 caracteres + opciones)
 * \param[in] opts Opciones específicas de la rutina (dependientes del algoritmo)
 * \param[in] n1 Primer parámetro dimensional del problema
 * \param[in] n2 Segundo parámetro dimensional del problema
 * \param[in] n3 Tercer parámetro dimensional del problema
 * \param[in] n4 Cuarto parámetro dimensional del problema
 * 
 * \return int Valor del parámetro solicitado o -1 para ispec no válido
 * 
 * \note
 * - El nombre se trunca/rellena a 16 caracteres (compatibilidad FORTRAN)
 * - Maneja múltiples esquemas de codificación de caracteres históricos
 * - Para ispec >= 12 delega en iparmq_reimpl()
 * 
 * \warning
 * - El buffer subnam es estático (no thread-safe)
 * - Comportamiento indefinido si name excede 16 caracteres sin NULL-terminator
 * - Algunos valores de ispec (ej. 4) están marcados como obsoletos
 * 
 * \par Procesamiento del nombre:
 * 1. Normalización a mayúsculas según codificación
 * 2. Extracción componentes (c1, c2, c3, c4)
 * 3. Detección de precisión (S/D/C/Z) y tipo de rutina
 * 
 * \par Ejemplos de uso:
 * \code
 * // Tamaño bloque óptimo para GEQRF
 * int nb = ilaenv_reimpl(1, "sgeqrf", "", 0, 0, 0, 0);
 * 
 * // Verificación soporte IEEE
 * int ieee_nan = ilaenv_reimpl(10, "", "", 0, 0, 0, 0);
 * \endcode
 * 
 * \see ieeeck_reimpl Para verificaciones IEEE 754
 * \see iparmq_reimpl Para parámetros 12-17
 */

int ilaenv_reimpl(int ispec, char* name, char* opts, int n1, int n2, int n3, int n4) {
    int ilaenv = 0;
    char subnam[17]; // 16 caracteres + terminador nulo
    char c1, c2[3], c3[4], c4[3];
    int sname, cname, twostage;

    // Copiar nombre y rellenar con espacios hasta 16 caracteres
    snprintf(subnam, sizeof(subnam), "%-16.16s", name);
    
    // Se supone que es ASCII, si fuera EBCDIC o Prime se debería ajustar si toupper no funciona correctamente
    
    for (int i = 0; i < 6 && subnam[i]; i++) { // Técnicamente tendría que convertir solo si el primer carácter es minúscula
        subnam[i] = toupper((unsigned char)subnam[i]); 
    }

    // Extraer componentes del nombre
    c1 = subnam[0];
    sname = (c1 == 'S' || c1 == 'D');
    cname = (c1 == 'C' || c1 == 'Z');

    if(!sname && !cname) {
        // Si no es S o D, entonces es C o Z
        return ilaenv;
    }

    snprintf(c2, sizeof(c2), "%-2.2s", subnam + 1); // Asegurarse de que c2 tenga 2 caracteres

    snprintf(c3, sizeof(c3), "%-3.3s", subnam + 3); // Asegurarse de que c3 tenga 3 caracteres

    snprintf(c4, sizeof(c4), "%-2.2s", c3+1);

    twostage = ((strlen(subnam) >= 11) && (subnam[10] == '2'));
    // Aunque la implementación anterior es la correcta, la línea de abajo es la única forma de que reconozca las funciones _2STAGE aunque no se va a usar
    //twostage = ((strlen(subnam) >= 11) &&(subnam[7] == '2')); // Índice 7 en C (posición 8 en 1-based)

    switch (ispec) {
        case 1:
            { // Tamaño de bloque óptimo
                ilaenv = 1;
                              
                if (strncmp(subnam + 1, "LAORH", 5) == 0) { // LAORHQR
                    ilaenv = 32;
                }else if (strncmp(c2, "GE", 2) == 0) { // GE
                    if (strncmp(c3, "TRF", 3) == 0) { // GE-TRF
                        ilaenv = 64;
                    }else if(strncmp(c3, "QRF", 3) == 0 ||
                            strncmp(c3, "RQF", 3) == 0 ||
                            strncmp(c3, "LQF", 3) == 0 ||
                            strncmp(c3, "QLF", 3) == 0) { // GE-QRF, GE-RQF, GE-LQF, GE-QLF
                        ilaenv = 32;
                    }else if (strncmp(c3, "QR ", 3) == 0) { // GE-QR
                        if (n3 == 1) { 
                            /* M*N case */
                            if ((n1 * n2 <= 131072) || (n1 <= 8192)){
                                ilaenv = n1;
                            }else {
                                ilaenv = 32768 / n2;
                            }
                        }else {
                            ilaenv = 1;
                        }
                    }else if (strncmp(c3, "LQ ", 3) == 0) { // GE-LQ
                        if (n3 == 2) {
                            /* M*N case */
                            if ((n1 * n2 <= 131072) || (n1 <= 8192)){
                                ilaenv = n1;
                            }else {
                                ilaenv = 32768 / n2;
                            }
                        }else {
                            ilaenv = 1;
                        }
                    }else if (strncmp(c3, "HRD", 3) == 0 ||
                             strncmp(c3, "BRD", 3) == 0 ||// GE-HRD, GE-BRD
                             strncmp(subnam + 3, "QP3RK", 4) == 0) {  // GE-QP3RK
                        ilaenv = 32;
                    }else if (strncmp(c3, "TRI", 3) == 0) { // GE-TRI
                        ilaenv = 64;
                    }
                }else if (strncmp(c2, "PO", 2) == 0) { // PO
                    if (strncmp(c3, "TRF", 3) == 0) { // PO-TRF
                        if (sname) {
                            ilaenv = 64;
                        } else{
                            ilaenv = 64;
                        }
                    }    
                }else if (strncmp(c2, "SY", 2) == 0) { // SY
                    if (strncmp(c3, "TRF", 3) == 0) { // SY-TRF
                        if(sname) {
                            ilaenv = (twostage ? 192 : 64);
                        }
                    }else if (sname && (strncmp(c3, "TRD", 3) == 0)) { // SY-TRD
                        ilaenv = 32;
                    }else if (sname && (strncmp(c3, "GST", 3) == 0)) { // SY-GST
                        ilaenv = 64;
                    }
                }else if (cname && strncmp(c2, "HE", 2) == 0) { // HE
                    if (strncmp(c3, "TRF", 3) == 0) { // HE-TRF
                        ilaenv = (twostage ? 192 : 64);
                    }else if (strncmp(c3, "TRD", 3) == 0) { // HE-TRD
                        ilaenv = 32;
                    }else if (strncmp(c3, "GST", 3) == 0) { // HE-GST
                        ilaenv = 64;
                    }
                }else if (sname && strncmp(c2, "OR", 2) == 0) { // OR
                    if (c3[0] == 'G' || c3[0] == 'M') { // OR-G, OR-M
                        if (strncmp(c4, "QR", 2) == 0 ||
                            strncmp(c4, "RQ", 2) == 0 ||
                            strncmp(c4, "LQ", 2) == 0 ||
                            strncmp(c4, "QL", 2) == 0 ||
                            strncmp(c4, "HR", 2) == 0 ||
                            strncmp(c4, "TR", 2) == 0 || // OR-GQR, OR-GRQ, OR-GLQ, OR-GQL, OR-GHR, OR-GTR
                            strncmp(c4, "BR", 2) == 0) { // OR-MQR, OR-MRQ, OR-MLQ, OR-MQL, OR-MHR, OR-MTR
                            ilaenv = 32;
                        }
                    }
                }else if (cname && strncmp(c2, "UN", 2) == 0) { // UN
                    if (c3[0] == 'G' || c3[0] == 'M') { // UN-G, UN-M
                        if (strncmp(c4, "QR", 2) == 0 ||
                            strncmp(c4, "RQ", 2) == 0 ||
                            strncmp(c4, "LQ", 2) == 0 ||
                            strncmp(c4, "QL", 2) == 0 ||
                            strncmp(c4, "HR", 2) == 0 ||
                            strncmp(c4, "TR", 2) == 0 || // UN-GQR, UN-GRQ, UN-GLQ, UN-GQL, UN-GHR, UN-GTR
                            strncmp(c4, "BR", 2) == 0) { // UN-MQR, UN-MRQ, UN-MLQ, UN-MQL, UN-MHR, UN-MTR
                            ilaenv = 32;
                        }
                    }
                }else if (strncmp(c2, "GB", 2) == 0) { // GB
                    if (strncmp(c3, "TRF", 3) == 0) { // GB-TRF
                        ilaenv = (sname ? (n4 <= 64 ? 1 : 32) : (n4 <= 64 ? 1 : 32));
                    }
                }
                else if (strncmp(c2, "PB", 2) == 0) { // PB
                    if (strncmp(c3, "TRF", 3) == 0) { // PB-TRF
                        ilaenv = (sname ? (n2 <= 64 ? 1 : 32) : (n2 <= 64 ? 1 : 32));
                    }
                }else if (strncmp(c2, "TR", 2) == 0) { // TR
                    if (strncmp(c3, "TRI", 3) == 0) { // TR-TRI
                        if(sname){
                            ilaenv = 64;
                        } else {
                            ilaenv = 64;
                        }
                    }else if (strncmp(c3, "EVC", 3) == 0) { // TR-EVC
                        if(sname) {
                            ilaenv = 64;
                        } else {
                            ilaenv = 64;
                        }
                    }else if (strncmp(c3, "SYL", 3) == 0) { // TR-SYL
                        if (sname) {
                            ilaenv = MIN(MAX(48, (int)((MIN(n1, n2) * 16) / 100)), 240);
                        }else {
                            ilaenv = MIN(MAX(24, (int)((MIN(n1, n2) * 8) / 100)), 80);
                        }
                    }
                }else if (strncmp(c2, "LA", 2) == 0) { // LA
                    if (strncmp(c3, "UUM", 3) == 0) { // LA-UUM
                        if(sname) {
                            ilaenv = 64;
                        } else {
                            ilaenv = 64;
                        }
                    }else if (strncmp(c3, "TRS", 3) == 0) { // LA-TRS
                        if(sname) {
                            ilaenv = 64;
                        } else {
                            ilaenv = 64;
                        }
                    }
                }else if (sname && strncmp(c2, "ST", 2) == 0) { // ST
                    if (strncmp(c3, "EBZ", 3) == 0) { // ST-EBZ
                        ilaenv = 1;
                    }
                }else if (strncmp(c2, "GG", 2) == 0) { // GG
                    ilaenv = 32;
                    if (strncmp(c3, "HD3", 3) == 0) { // GG-HD3
                        if (sname) {
                            ilaenv = 32;
                        } else {
                            ilaenv = 32;
                        }
                    }
                }

                break;
            }
            
        case 2: // Tamaño mínimo de bloque
        {
            ilaenv = 2;
            if (strncmp(c2, "GE", 2) == 0) { // GE
                if (strncmp(c3, "QRF", 3) == 0 ||
                    strncmp(c3, "RQF", 3) == 0 ||
                    strncmp(c3, "LQF", 3) == 0 ||
                    strncmp(c3, "QLF", 3) == 0 || // GE-QRF, GE-RQF, GE-LQF, GE-LQF
                    strncmp(c3, "HRD", 3) == 0 ||
                    strncmp(c3, "BRD", 3) == 0 ||
                    strncmp(c3, "TRI", 3) == 0 || // GE-HRD, GE-BRD, GE-STF
                    strncmp(subnam+3, "QP3RK", 4) == 0){  // GE-QP3RK
                    if(sname) {
                        ilaenv = 2;
                    } else {
                        ilaenv = 2;
                    }
                }
            } else if (strncmp(c2, "SY", 2) == 0){ // SY
                if(strncmp(c3, "TRF", 3) == 0) { // SY-TRF
                    ilaenv = 8; // SY-TRF
                } else if(sname && strncmp(c3, "TRD", 3) == 0){
                    ilaenv = 2; // SY-TRD
                }    
            } else if (cname && strncmp(c2, "HE", 2) == 0 && strncmp(c3, "TRD", 3) == 0) {
                ilaenv = 2; // HE-TRD
            
            } else if (sname && strncmp(c2, "OR", 2) == 0){
                if(c3[0] == 'G' || c3[0] == 'M'){ // OR-G || OR-M
                    if (strncmp(c4, "QR", 2) == 0 ||
                        strncmp(c4, "RQ", 2) == 0 ||
                        strncmp(c4, "LQ", 2) == 0 ||
                        strncmp(c4, "QL", 2) == 0 ||
                        strncmp(c4, "HR", 2) == 0 ||
                        strncmp(c4, "TR", 2) == 0 || // OR-M-QR, OR-M-RQ, OR-M-LQ, OR-M-QL, OR-M-HR, OR-M-TR, OR-M-BR
                        strncmp(c4, "BR", 2) == 0){  // OR-G-QR, OR-G-RQ, OR-G-LQ, OR-G-QL, OR-G-HR, OR-G-TR, OR-G-BR
                        ilaenv = 2;
                    }
                }
            } else if (cname && strncmp(c2, "UN", 2) == 0){
                if(c3[0] == 'G' || c3[0] == 'M'){ // UN-G || UN-M
                    if (strncmp(c4, "QR", 2) == 0 ||
                        strncmp(c4, "RQ", 2) == 0 ||
                        strncmp(c4, "LQ", 2) == 0 ||
                        strncmp(c4, "QL", 2) == 0 ||
                        strncmp(c4, "HR", 2) == 0 ||
                        strncmp(c4, "TR", 2) == 0 || // UN-M-QR, UN-M-RQ, UN-M-LQ, UN-M-QL, UN-M-HR, UN-M-TR, UN-M-BR
                        strncmp(c4, "BR", 2) == 0){  // UN-G-QR, UN-G-RQ, UN-G-LQ, UN-G-QL, UN-G-HR, UN-G-TR, UN-G-BR
                        ilaenv = 2;
                    }
                }
                
            } else if (strncmp(c2, "GG", 2) == 0) { // GG
                ilaenv = 2; 
                if(strncmp(c3, "HD3", 3) == 0){ // GG-HD3
                    ilaenv = 2;
                }
            }
            break;
        }
        
        case 3: // Punto de corte
            {   ilaenv = 0;
                if (strncmp(c2, "GE", 2) == 0) { // GE
                    if (strncmp(c3, "QRF", 3) == 0 ||
                        strncmp(c3, "RQF", 3) == 0 ||
                        strncmp(c3, "LQF", 3) == 0 ||
                        strncmp(c3, "QLF", 3) == 0 || // GE-QRF, GE-RQF, GE-LQF, GE-QLF
                        strncmp(c3, "HRD", 3) == 0 ||
                        strncmp(c3, "BRD", 3) == 0 || // GE-HRD, GE-BRD
                        strncmp(subnam+3, "QP3RK", 4) == 0) { // GE-QP3RK
                        if(sname) {
                            ilaenv = 128;
                        } else {
                            ilaenv = 128;

                        }
                    }
                } else if (strncmp(c2, "SY", 2) == 0 ) { // SY
                    if(sname && strncmp(c3, "TRD", 3) == 0){ // SY-TRD
                        ilaenv = 32;
                    }
                } else if (cname && strncmp(c2, "HE", 2) == 0) { // HE
                    if(strncmp(c3, "TRD", 3) == 0){ // HE-TRD
                        ilaenv = 32; 
                    }
                } else if (sname && strncmp(c2, "OR", 2) == 0) { // OR
                    if(c3[0] == 'G'){ // OR-G 
                        if (strncmp(c4, "QR", 2) == 0 ||
                            strncmp(c4, "RQ", 2) == 0 ||
                            strncmp(c4, "LQ", 2) == 0 ||
                            strncmp(c4, "QL", 2) == 0 ||
                            strncmp(c4, "HR", 2) == 0 ||
                            strncmp(c4, "TR", 2) == 0 ||
                            strncmp(c4, "BR", 2) == 0){  // OR-G-QR, OR-G-RQ, OR-G-LQ, OR-G-QL, OR-G-HR, OR-G-TR, OR-G-BR
                            ilaenv = 128;
                        }
                    }
                } else if (cname && strncmp(c2, "UN", 2) == 0) { // OR
                    if(c3[0] == 'G'){ // OR-G 
                        if (strncmp(c4, "QR", 2) == 0 ||
                            strncmp(c4, "RQ", 2) == 0 ||
                            strncmp(c4, "LQ", 2) == 0 ||
                            strncmp(c4, "QL", 2) == 0 ||
                            strncmp(c4, "HR", 2) == 0 ||
                            strncmp(c4, "TR", 2) == 0 ||
                            strncmp(c4, "BR", 2) == 0){  // UN-G-QR, UN-G-RQ, UN-G-LQ, UN-G-QL, UN-G-HR, UN-G-TR, UN-G-BR
                            ilaenv = 128;
                        }
                    }
                } else if (strncmp(c2, "GG", 2) == 0) {
                    ilaenv = 128; // GG-HD3
                    if (strncmp(c3, "HD3", 3) == 0) {
                        ilaenv = 128;

                    }
                }
                break;
            }

        case 4: // Número de shifts (DEPRECATED)
            {
                ilaenv = 6;
                break;
            }
        case 5: // Mínima columna bloque
            {
                ilaenv = 2;
                break;
            }
        case 6: // crossover SVD
            {    
                ilaenv = (int)(fmin(n1, n2) * 1.6);
                break;
            }
        case 7: // núcleos CPU
            {
                ilaenv = 1;
                break;
            }
        case 8: // crossover multishift QR
            {
                ilaenv = 50;
                break;
            }
        case 9: // tamaño máximo subproblemas
            {
                ilaenv = 25;
                break;
            }
        case 10: // Verificación IEEE NaN
            {
                float zero = 0.0f;
                float one = 1.0f;
                ilaenv = ieeeck_reimpl(1, &zero, &one);
            }
            break;

        case 11: // Verificación de infinito
            {
                float zero = 0.0f;
                float one = 1.0f;
                ilaenv = ieeeck_reimpl(0, &zero, &one);
            }
            break;
        
        case 12: 
        case 13: 
        case 14: 
        case 15: 
        case 16: 
        case 17: 
            {
                char subnam_fortran[17];
                snprintf(subnam_fortran, sizeof(subnam_fortran), "%-16.16s", subnam);
                ilaenv = iparmq_reimpl(ispec, subnam_fortran, opts, n1, n2, n3, n4);
                break;
            }
        default:
                ilaenv = -1;
                break;
    }

    return ilaenv;
}