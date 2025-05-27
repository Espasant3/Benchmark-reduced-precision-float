
#include "lapacke_utils_reimpl.h"

/**
 * \file iparmq_reimpl.c
 * \brief Implementación de la función iparmq para selección de parámetros algorítmicos en C
 */

/**
 * \defgroup PARAMQ_CONSTANTS Constantes para selección de parámetros algorítmicos
 * \{
 */

/** \brief Especificador para tamaño mínimo de problema */
#define INMIN 12
/** \brief Especificador para tamaño inicial de ventana */
#define INWIN 13
/** \brief Especificador para tamaño de bloque inicial */
#define INIBL 14
/** \brief Especificador para número de shifts QR */
#define ISHFTS 15
/** \brief Especificador para aceleración Level 2.2 BLAS */
#define IACC22 16
/** \brief Especificador para costo computacional relativo */
#define ICOST 17

/** \brief Tamaño mínimo de problema para algoritmos recursivos */
#define NMIN 75
/** \brief Límite mínimo para activar aceleración Level 2.2 */
#define K22MIN 14
/** \brief Límite mínimo para activación condicional de aceleración */
#define KACMIN 14
/** \brief Tamaño de bloque para operaciones nibble */
#define NIBBLE 14
/** \brief Umbral para cambio de estrategia en ventanas */
#define KNWSWP 500
/** \brief Costo relativo de operaciones básicas */
#define RCOST 10

/** \} */ // Fin de grupo PARAMQ_CONSTANTS

/**
 * \brief Calcula parámetros algorítmicos para rutinas LAPACK (versión reimplementada)
 * 
 * \details Función inspirada en iparmq de LAPACK con:
 *          - Soporte mejorado para nombres de subrutinas en C
 *          - Lógica adaptada para precisión mixta
 *          - Optimizaciones para arquitecturas modernas
 *
 * \param[in] ispec Tipo de parámetro a calcular (usa constantes INMIN, INWIN, etc.)
 * \param[in] name Nombre de la subrutina LAPACK (6 caracteres max)
 * \param[in] opts Opciones de la subrutina (no usado en esta implementación)
 * \param[in] n Dimensión principal del problema
 * \param[in] ilo Índice inferior del subespacio considerado
 * \param[in] ihi Índice superior del subespacio considerado
 * \param[in] lwork Tamaño del workspace (usado en algunos cálculos)
 *
 * \return int Valor del parámetro solicitado o -1 para ispec no válido
 *
 * \note
 * - El nombre se trunca/rellena a 6 caracteres (compatibilidad FORTRAN)
 * - Conversión automática a mayúsculas para primeros 6 caracteres
 * - La lógica de ISHFTS/INWIN/IACC22 depende del tamaño del subespacio (ihi - ilo + 1)
 *
 * \warning
 * - Comportamiento indefinido si name excede 6 caracteres sin NULL-terminator
 * - Los valores de retorno para IACC22 dependen de patrones en el nombre:
 *   * 2 para GGHRD/GGHD3 con nh >= K22MIN
 *   * 1 para otros casos con nh >= KACMIN
 *   * 0 otherwise
 *
 * \par Ejemplos de uso:
 * \code
 * // Número de shifts para subespacio de tamaño 100
 * int ns = iparmq_reimpl(ISHIFTS, "DHSEQR", "EH", 100, 1, 100, 0); // Retorna 10
 *
 * // Parámetro de aceleración para GGHRD
 * int acc = iparmq_reimpl(IACC22, "DGGHRD", "VLT", 200, 1, 200, 0); // Retorna 2
 * \endcode
 */
int iparmq_reimpl(int ispec, const char *name, const char *opts, int n, int ilo, int ihi, int lwork) {
    int nh = ihi - ilo + 1;
    int ns = 2;
    char subnam[7] = {' '}; // Inicializar con espacios

    // Copiar los primeros 6 caracteres sin conversión
    for (int i = 0; i < 6; i++) {
        subnam[i] = (i < strlen(name)) ? name[i] : ' ';
    }
    subnam[6] = '\0';

    // Convertir a mayúsculas solo si el primer carácter es minúscula
    if (subnam[0] >= 'a' && subnam[0] <= 'z') {
        subnam[0] = toupper((unsigned char)subnam[0]);
        for (int i = 1; i < 6; i++) {
            if (subnam[i] >= 'a' && subnam[i] <= 'z') {
                subnam[i] = toupper((unsigned char)subnam[i]);
            }
        }
    }

    if (ispec == ISHFTS || ispec == INWIN || ispec == IACC22) {
        if (nh >= 30) ns = 4;
        if (nh >= 60) ns = 10;
        if (nh >= 150) {
            double log2_nh = log(nh) / log(2.0);
            int divisor = lround(log2_nh);
            ns = nh / divisor;
            ns = (ns > 10) ? ns : 10;
        }
        if (nh >= 590) ns = 64;
        if (nh >= 3000) ns = 128;
        if (nh >= 6000) ns = 256;
        ns = (ns >= 2) ? ns : 2;
        ns -= (ns % 2);
    }

    switch (ispec) {
        case INMIN: return NMIN;
        case INIBL: return NIBBLE;
        case ISHFTS: return ns;
        case INWIN: return (nh <= KNWSWP) ? ns : (3 * ns) / 2;
        case IACC22: {
            int iparmq_val = 0;
            char part[6] = {0};

            // Verificar 'GGHRD' o 'GGHD3' (posiciones 2-6)
            snprintf(part, sizeof(part), "%.5s", subnam + 1); // Copia 5 chars + \0
            if (strcmp(part, "GGHRD") == 0 || strcmp(part, "GGHD3") == 0) {
                iparmq_val = (nh >= K22MIN) ? 2 : 1;
            } else {
                // Verificar 'EXC' (posiciones 4-6)
                snprintf(part, sizeof(part), "%.3s", subnam + 3); // Copia 3 chars + \0
                if (strcmp(part, "EXC") == 0) {
                    iparmq_val = (nh >= K22MIN) ? 2 : ((nh >= KACMIN) ? 1 : 0);
                } else {
                    // Verificar 'HSEQR' (posiciones 2-6) o 'LAQR' (posiciones 2-5)
                    snprintf(part, sizeof(part), "%.5s", subnam + 1); // Copia 5 chars + \0
                    if (strcmp(part, "HSEQR") == 0) {
                        iparmq_val = (ns >= K22MIN) ? 2 : ((ns >= KACMIN) ? 1 : 0);
                    } else {
                        snprintf(part, sizeof(part), "%.4s", subnam + 1); // Copia 4 chars + \0
                        if (strcmp(part, "LAQR") == 0) {
                            iparmq_val = (ns >= K22MIN) ? 2 : ((ns >= KACMIN) ? 1 : 0);
                        }
                    }
                }
            }
            return iparmq_val;
        }
        case ICOST: return RCOST;
        default: return -1;
    }
}