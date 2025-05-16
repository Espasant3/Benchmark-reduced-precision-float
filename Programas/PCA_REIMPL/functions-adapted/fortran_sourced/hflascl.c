
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file hflascl.c
 * \brief Escalado de matrices en _Float16 con tipos y estructuras variadas
 */

/**
 * \brief Escala una matriz en _Float16 para cambiar su rango numérico
 * 
 * \details Versión _Float16 de LAPACK's SLASCL. Escala elementos de la matriz:
 *          \f[ A := (CTO/CFROM) \cdot A \f]
 *          Soporta múltiples tipos de matriz con manejo seguro de overflow/underflow.
 * 
 * \param[in] TYPE  Tipo de matriz (case-sensitive):
 *                 - 'G': General (M x N)
 *                 - 'L': Triangular inferior
 *                 - 'U': Triangular superior
 *                 - 'H': Hessenberg superior
 *                 - 'B': Banda simétrica (KL=KU)
 *                 - 'Q': Banda simétrica (KL=KU) en formato packed
 *                 - 'Z': Banda general (KL subdiagonales, KU superdiagonales)
 * \param[in] KL    Número de subdiagonales (solo tipos 'B','Z')
 * \param[in] KU    Número de superdiagonales (solo tipos 'B','Z')
 * \param[in] CFROM Factor de escala original (≠0, no NaN)
 * \param[in] CTO   Factor de escala objetivo (≠NaN)
 * \param[in] M     Filas de la matriz
 * \param[in] N     Columnas de la matriz
 * \param[in,out] A Matriz _Float16 a escalar
 * \param[in] LDA   Leading dimension de A
 * \param[out] INFO Código de salida:
 *                 - 0: éxito
 *                 - <0: error en parámetro i
 * 
 * \note
 * - Algoritmo multi-paso para evitar desbordamientos:
 *   1. Calcula SMLNUM = FP16_TRUE_MIN/FP16_EPSILON
 *   2. Aplica escalado iterativo hasta alcanzar rango seguro
 * - Soporte para 7 tipos de almacenamiento matricial
 * - Compatible con matrices vacías (M=0 o N=0)
 * 
 * \warning
 * - CFROM=0 o NaN causa error -4
 * - KL/KU inconsistentes con TYPE causan error -2/-3
 * - LDA insuficiente para el tipo causa error -9
 * - No verifica solapamiento de memoria en operaciones
 * 
 * \par Tipos de matriz y patrones de acceso:
 * | TYPE | Patrón                     | Bandas          |
 * |------|----------------------------|-----------------|
 * | G    | Todos los elementos        | M x N completa  |
 * | L    | i >= j                     | Triangular inf  |
 * | U    | i <= j                     | Triangular sup  |
 * | H    | i <= j+1                   | Hessenberg sup  |
 * | B    | max(0,j-KL) <= i <= j+KU   | Banda simétrica |
 * | Z    | j-KL <= i <= j+KU          | Banda general   |
 * 
 * \example
 * _Float16 A[4][4] = {...}; // Matriz 4x4
 * int info;
 * hflascl('G', 0, 0, 2.0F16, 4.0F16, 4, 4, A[0], 4, &info); // Escala x2
 * 
 * \see hflamch Para obtención de constantes máquina
 * \see LAPACKE_xerbla Para manejo de errores
 * \see LAPACK_SISNAN Para detección de NaN
 */

void hflascl(char TYPE, lapack_int KL, lapack_int KU, _Float16 CFROM, _Float16 CTO,
             lapack_int M, lapack_int N, _Float16 *A, lapack_int LDA, lapack_int *INFO) {
                
    bool DONE = false;
    int ITYPE, I, J;
    _Float16 BIGNUM, CFROM1, CFROMC = CFROM, CTO1, CTOC = CTO, MUL, SMLNUM;

    /* Validación de parámetros */
    *INFO = 0;
    switch (TYPE) {
        case 'G': ITYPE = 0; break;
        case 'L': ITYPE = 1; break;
        case 'U': ITYPE = 2; break;
        case 'H': ITYPE = 3; break;
        case 'B': ITYPE = 4; break;
        case 'Q': ITYPE = 5; break;
        case 'Z': ITYPE = 6; break;
        default:  ITYPE = -1; break; // Caso no reconocido
    }


    if (ITYPE == -1)
        *INFO = -1;
    else if (CFROM == 0.0F16 || LAPACK_HFISNAN(CFROM))
        *INFO = -4;
    else if (LAPACK_HFISNAN(CTO))
        *INFO = -5;
    else if (M < 0)
        *INFO = -6;
    else if (N < 0 || (ITYPE == 4 && N != M) || (ITYPE == 5 && N != M))
        *INFO = -7;
    else if ((ITYPE <= 3 && LDA < MAX(1, M)))
        *INFO = -9;
    else if (ITYPE >= 4) {
        if (KL < 0 || KL > MAX(M - 1, 0))
            *INFO = -2;
        else if (KU < 0 || KU > ((N > 0) ? N - 1 : 0) ||
                 ((ITYPE == 4 || ITYPE == 5) && KL != KU))
            *INFO = -3;
        else if ((ITYPE == 4 && LDA < (KL + 1)) ||
                 (ITYPE == 5 && LDA < (KU + 1)) ||
                 (ITYPE == 6 && LDA < (2 * KL + KU + 1)))
            *INFO = -9;
    }

    if (*INFO != 0) {
        LAPACKE_xerbla("HFLASCL", *INFO);
        return;
    }
    if (N == 0 || M == 0)
        return;

    SMLNUM = hflamch('S');
    BIGNUM = 1.0F16 / SMLNUM;

    do {
        CFROM1 = CFROMC * SMLNUM;
        if (CFROM1 == CFROMC) {  // CFROMC es infinito
            MUL = CTOC / CFROMC;
            DONE = true;
        } else {
            CTO1 = CTOC / BIGNUM;
            if (CTO1 == CTOC) {  // CTOC es cero o infinito
                MUL = CTOC;
                DONE = true;
                CFROMC = 1.0F16;
            } else if (abs(CFROM1) > abs(CTOC) && CTOC != 0.0F16) {
                MUL = SMLNUM;
                CFROMC = CFROM1;
            } else if (abs(CTO1) > abs(CFROMC)) {
                MUL = BIGNUM;
                CTOC = CTO1;
            } else {
                MUL = CTOC / CFROMC;
                DONE = true;
            }
        }

        if (DONE && MUL == 1.0F16)
            return;

        /* Aplicar escalado si es necesario */
        if (!DONE || MUL != 1.0F16) {
            switch (ITYPE) {
                case 0:  // Matriz completa
                    for (J = 0; J < N; J++)
                        for (I = 0; I < M; I++)
                            A[I + J * LDA] *= MUL;
                    break;

                case 1:  // Triangular inferior
                    for (J = 0; J < N; J++)
                        for (I = J; I < M; I++)
                            A[I + J * LDA] *= MUL;
                    break;

                case 2:  // Triangular superior
                    for (J = 0; J < N; J++) {
                        int max_i = (J < M) ? J + 1 : M;
                        for (I = 0; I < max_i; I++)
                            A[I + J * LDA] *= MUL;
                    }
                    break;

                case 3: {  // Upper Hessenberg
                    for (J = 0; J < N; J++) {
                        int max_i = (J + 2 < M) ? J + 2 : M;
                        for (I = 0; I < max_i; I++)
                            A[I + J * LDA] *= MUL;
                    }
                    break;
                }

                case 4: {  // Banda simétrica inferior
                    int K3 = KL + 1;
                    for (J = 0; J < N; J++) {
                        int max_i = (K3 < (N - J)) ? K3 : (N - J);
                        for (I = 0; I < max_i; I++)
                            A[I + J * LDA] *= MUL;
                    }
                    break;
                }

                case 5: {  // Banda simétrica superior
                    int K1 = KU + 2;
                    int K3 = KU + 1;
                    for (J = 0; J < N; J++) {
                        int start_i_f = (K1 - (J + 1) > 1) ? (K1 - (J + 1)) : 1;
                        int start_i = start_i_f - 1;
                        for (I = start_i; I < K3; I++)
                            A[I + J * LDA] *= MUL;
                    }
                    break;
                }

                case 6: {  // Banda general (KL subdiagonales, KU superdiagonales)
                    const int total_bands = KL + KU + 1;
                    for (lapack_int col = 0; col < N; ++col) {
                        // Rango de filas a escalar en la columna actual
                        const int first_row = MAX(0, col - KU);           // Primera fila en la matriz original
                        const int last_row = MIN(M - 1, col + KL);        // Última fila en la matriz original
                        
                        // Convertir a índices de almacenamiento banda
                        const int storage_start = KU + (first_row - col); // Fila en el arreglo banda
                        const int storage_end = KU + (last_row - col);
                        
                        for (int storage_row = storage_start; storage_row <= storage_end; ++storage_row) {
                            A[storage_row + col * LDA] *= MUL;
                        }
                    }
                    break;
                }
            }
        }
    } while (!DONE);
}