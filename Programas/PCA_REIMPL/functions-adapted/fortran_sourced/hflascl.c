
#include "lapacke_utils_reimpl.h"

/**
 * \file hflascl.c
 * \brief Escalado de matrices en lapack_float con tipos y estructuras variadas
 */

/**
 * \brief Escala una matriz en lapack_float para cambiar su rango numérico
 * 
 * \details Versión lapack_float de LAPACK's SLASCL. Escala elementos de la matriz:
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
 * \param[in,out] A Matriz lapack_float a escalar
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
 * lapack_float A[4][4] = {...}; // Matriz 4x4
 * int info;
 * hflascl('G', 0, 0, 2.0, 4.0, 4, 4, A[0], 4, &info); // Escala x2
 * 
 * \see hflamch_half_precision Para obtención de constantes máquina
 * \see LAPACKE_xerbla Para manejo de errores
 * \see LAPACK_HFISNAN Para detección de NaN
 */

void hflascl(char TYPE, lapack_int KL, lapack_int KU, lapack_float CFROM, lapack_float CTO,
             lapack_int M, lapack_int N, lapack_float *A, lapack_int LDA, lapack_int *INFO) {
                
    bool DONE = false;
    int ITYPE, I, J;
    int K1, K2, K3, K4;
    lapack_float BIGNUM, CFROM1, CFROMC = CFROM, CTO1, CTOC = CTO, MUL, SMLNUM;

    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;

    /* Validación de parámetros */
    *INFO = 0;
switch (TYPE) {
    case 'G':
    case 'g':
        ITYPE = 0; break;
    case 'L':
    case 'l':
        ITYPE = 1; break;
    case 'U':
    case 'u':
        ITYPE = 2; break;
    case 'H':
    case 'h':
        ITYPE = 3; break;
    case 'B':
    case 'b':
        ITYPE = 4; break;
    case 'Q':
    case 'q':
        ITYPE = 5; break;
    case 'Z':
    case 'z':
        ITYPE = 6; break;
    default:
        ITYPE = -1; break;
}


    if (ITYPE == -1)
        *INFO = -1;
    else if (CFROM == ZERO || LAPACK_HFISNAN(CFROM))
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

    SMLNUM = hflamch_half_precision('S');
    BIGNUM = ONE / SMLNUM;

    do {
        CFROM1 = CFROMC * SMLNUM;
        if (CFROM1 == CFROMC) {  // CFROMC es infinito
            MUL = CTOC / CFROMC;
            DONE = true;
            CTO1 = CTOC;
        } else {
            CTO1 = CTOC / BIGNUM;
            if (CTO1 == CTOC) {  // CTOC es cero o infinito
                MUL = CTOC;
                DONE = true;
                CFROMC = ONE;
            } else if (ABS_half_precision(CFROM1) > ABS_half_precision(CTOC) && CTOC != ZERO) {
                MUL = SMLNUM;
                DONE = false;
                CFROMC = CFROM1;
            } else if (ABS_half_precision(CTO1) > ABS_half_precision(CFROMC)) {
                MUL = BIGNUM;
                DONE = false;
                CTOC = CTO1;
            } else {
                MUL = CTOC / CFROMC;
                DONE = true;
                if(MUL == ONE){
                    return;
                }
            }
        }
        
        /* Aplicar escalado si es necesario */
        if (!DONE || MUL != ONE) {
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
                        int max_i = MIN(J, M);
                        for (I = 0; I < max_i; I++)
                            A[I + J * LDA] *= MUL;
                    }
                    break;

                case 3: {  // Upper Hessenberg
                    for (J = 0; J < N; J++) {
                        int max_i = MIN(J + 1, M);
                        for (I = 0; I < max_i; I++)
                            A[I + J * LDA] *= MUL;
                    }
                    break;
                }

                case 4: {  // Banda simétrica inferior
                    K3 = KL + 1;
                    K4 = N + 1;
                    for (J = 0; J < N; J++) {
                        int max_i = MIN(K3, K4 - J);
                        for (I = 0; I < max_i; I++)
                            A[I + J * LDA] *= MUL;
                    }
                    break;
                }

                case 5: {  // Banda simétrica superior
                    K1 = KU + 2;
                    K3 = KU + 1;
                    for (J = 0; J < N; J++) {
                        int start_i = MAX(K1 - (J + 1), 1) - 1;
                        for (I = start_i; I < K3; I++)
                            A[I + J * LDA] *= MUL;
                    }
                    break;
                }

                case 6: {  // Banda general (KL subdiagonales, KU superdiagonales)

                    K1 = KL + KU + 2;
                    K2 = KL + 1;
                    K3 = 2 * KL + KU + 1;
                    K4 = KL + KU + 1 + M;

                    for (int j = 0; j < N; j++) {
                        /* Convertimos el índice de columna: Fortran j es (j+1) en C */
                        int i_start = MAX(K1 - (j + 1), K2) - 1; /* -1 para pasar a 0-based */
                        int i_end   = MIN(K3, K4 - (j + 1)) - 1;
                        for (int i = i_start; i <= i_end; i++) {
                            /* Accedemos con el cálculo típico en almacenamiento column-major */
                            A[i + j * LDA] *= MUL;
                        }
                    }
                    break;
                }
            }
        }
    } while (!DONE);
}