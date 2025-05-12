
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file hflasr.c
 * \brief Aplica rotaciones planas a una matriz en _Float16
 */

/**
 * \brief Aplica una secuencia de rotaciones planas a una matriz en _Float16
 * 
 * \details Versión _Float16 de LAPACK's SLASR. Aplica rotaciones definidas por los
 *          ángulos c[i] = cos(theta_i) y s[i] = sin(theta_i) desde la izquierda/derecha
 *          según los parámetros de configuración.
 * 
 * \param[in] side  Lado de aplicación (case-insensitive):
 *                 - 'L': Aplica rotaciones desde la izquierda (afecta filas)
 *                 - 'R': Aplica rotaciones desde la derecha (afecta columnas)
 * \param[in] pivot Punto pivote de las rotaciones (case-insensitive):
 *                 - 'V': (Variable) Rotaciones entre elementos consecutivos (i, i+1)
 *                 - 'T': (Top) Rotaciones pivoteando desde el primer elemento
 *                 - 'B': (Bottom) Rotaciones pivoteando desde el último elemento
 * \param[in] direct Dirección de aplicación (case-insensitive):
 *                 - 'F': (Forward) De primero a último elemento
 *                 - 'B': (Backward) De último a primero elemento
 * \param[in] m     Número de filas de la matriz A
 * \param[in] n     Número de columnas de la matriz A
 * \param[in] c     Array _Float16 de cosenos (longitud m-1 si side='L', n-1 si side='R')
 * \param[in] s     Array _Float16 de senos (misma dimensión que c)
 * \param[in,out] a Matriz _Float16 column-major (lda >= max(1,m))
 * \param[in] lda   Leading dimension de la matriz A
 * 
 * \note
 * - Las rotaciones se aplican en pares: Para cada i, se usa c[i] y s[i]
 * - Para side='L', modifica filas de A; para side='R', modifica columnas
 * - El pivote 'V' requiere m-1 (left) o n-1 (right) rotaciones
 * - El orden de aplicación depende de direct: F=forward, B=backward
 * 
 * \warning
 * - No valida consistencia entre tamaño de c/s y parámetros m/n
 * - Comportamiento indefinido si pivot no es 'V','T','B'
 * - No verifica solapamiento de memoria en a
 * 
 * \par Ejemplo (side='L', pivot='V', direct='F'):
 * Aplica rotaciones entre filas consecutivas desde arriba hacia abajo:
 * \code
 * for j = 0 to m-2:
 *     A[j:j+1, :] = [c[j]  -s[j]] * A[j:j+1, :]
 *                   [s[j]   c[j]]
 * \endcode
 * 
 * \par Ejemplo (side='R', pivot='B', direct='B'):
 * Aplica rotaciones desde la última columna hacia la primera:
 * \code
 * for j = n-2 downto 0:
 *     A[:, j:j+1] = A[:, j:j+1] * [c[j]   s[j]]
 *                                  [-s[j]  c[j]]
 * \endcode
 * 
 * \see LAPACKE_xerbla Para reporte de errores
 * \see lsame_reimpl Para comparación case-insensitive
 */

void hflasr(char side, char pivot, char direct, lapack_int m, lapack_int n,
           const _Float16* c, const _Float16* s, _Float16* a, lapack_int lda) {

    const _Float16 ONE = 1.0F16;
    const _Float16 ZERO = 0.0F16;
    lapack_int i, j;
    _Float16 ctemp, stemp, temp;
    lapack_int info = 0;

    // Validación de parámetros
    if (!lsame_reimpl(side, 'L') && !lsame_reimpl(side, 'R')) {
        info = 1;
    } else if (!lsame_reimpl(pivot, 'V') && !lsame_reimpl(pivot, 'T') && !lsame_reimpl(pivot, 'B')) {
        info = 2;
    } else if (!lsame_reimpl(direct, 'F') && !lsame_reimpl(direct, 'B')) {
        info = 3;
    } else if (m < 0) {
        info = 4;
    } else if (n < 0) {
        info = 5;
    } else if (lda < MAX(1, n)) { // Como son matrices row-major, se valida n (aunque al ser cuadradas, m=n)
        info = 9;
    }

    if (info != 0) {
        LAPACKE_xerbla("hflasr", info);
        return;
    }

    // Retorno rápido si no hay trabajo
    if (m == 0 || n == 0) return;

    // Aplicar rotaciones desde la izquierda
    if (lsame_reimpl(side, 'L')) {
        if (lsame_reimpl(pivot, 'V')) {
            if (lsame_reimpl(direct, 'F')) {
                for (j = 0; j < m - 1; ++j) {
                    ctemp = c[j];
                    stemp = s[j];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < n; ++i) {
                            temp = a[(j+1)*lda + i];
                            a[(j+1)*lda + i] = ctemp * temp - stemp * a[j*lda + i];
                            a[j*lda + i] = stemp * temp + ctemp * a[j*lda + i];
                        }
                    }
                }
            } else {
                for (j = m - 2; j >= 0; --j) {
                    ctemp = c[j];
                    stemp = s[j];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < n; ++i) {
                            temp = a[(j+1)*lda + i];
                            a[(j+1)*lda + i] = ctemp * temp - stemp * a[j*lda + i];
                            a[j*lda + i] = stemp * temp + ctemp * a[j*lda + i];
                        }
                    }
                }
            }
        } else if (lsame_reimpl(pivot, 'T')) {
            if (lsame_reimpl(direct, 'F')) {
                for (j = 1; j < m; ++j) {
                    ctemp = c[j - 1];
                    stemp = s[j - 1];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < n; ++i) {
                            temp = a[j*lda + i];
                            a[j*lda + i] = ctemp * temp - stemp * a[i];
                            a[i] = stemp * temp + ctemp * a[i];
                        }
                    }
                }
            } else {
                for (j = m - 1; j >= 1; --j) {
                    ctemp = c[j - 1];
                    stemp = s[j - 1];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < n; ++i) {
                            temp = a[j*lda + i];
                            a[j*lda + i] = ctemp * temp - stemp * a[i];
                            a[i] = stemp * temp + ctemp * a[i];
                        }
                    }
                }
            }
        } else if (lsame_reimpl(pivot, 'B')) {
            if (lsame_reimpl(direct, 'F')) {
                for (j = 0; j < m - 1; ++j) {
                    ctemp = c[j];
                    stemp = s[j];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < n; ++i) {
                            temp = a[j*lda + i];
                            a[j*lda + i] = stemp * a[(m-1)*lda + i] + ctemp * temp;
                            a[(m-1)*lda + i] = ctemp * a[(m-1)*lda + i] - stemp * temp;
                        }
                    }
                }
            } else {
                for (j = m - 2; j >= 0; --j) {
                    ctemp = c[j];
                    stemp = s[j];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < n; ++i) {
                            temp = a[j*lda + i];
                            a[j*lda + i] = stemp * a[(m-1)*lda + i] + ctemp * temp;
                            a[(m-1)*lda + i] = ctemp * a[(m-1)*lda + i] - stemp * temp;
                        }
                    }
                }
            }
        }
    } else { // Aplicar rotaciones desde la derecha
        if (lsame_reimpl(pivot, 'V')) {
            if (lsame_reimpl(direct, 'F')) {
                for (j = 0; j < n - 1; ++j) {
                    ctemp = c[j];
                    stemp = s[j];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < m; ++i) {
                            temp = a[i*lda + (j+1)];
                            a[i*lda + j+1] = ctemp * temp - stemp * a[i*lda + j];
                            a[i*lda + j] = stemp * temp + ctemp * a[i*lda + j];
                        }
                    }
                }
                
            } else {
                for (j = n - 2; j >= 0; --j) {
                    ctemp = c[j];
                    stemp = s[j];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < m; ++i) {
                            temp = a[i*lda + (j+1)];
                            a[i*lda + j+1] = ctemp * temp - stemp * a[i*lda + j];
                            a[i*lda + j] = stemp * temp + ctemp * a[i*lda + j];
                        }
                    }
                }
            }
        } else if (lsame_reimpl(pivot, 'T')) {
            if (lsame_reimpl(direct, 'F')) {
                for (j = 1; j < n; ++j) {
                    ctemp = c[j - 1];
                    stemp = s[j - 1];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < m; ++i) {
                            temp = a[i*lda + j];
                            a[i*lda + j] = ctemp * temp - stemp * a[i*lda];
                            a[i*lda] = stemp * temp + ctemp * a[i*lda];
                        }
                    }
                }
            } else {
                for (j = n - 1; j >= 1; --j) {
                    ctemp = c[j - 1];
                    stemp = s[j - 1];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < m; ++i) {
                            temp = a[i*lda + j];
                            a[i*lda + j] = ctemp * temp - stemp * a[i*lda];
                            a[i*lda] = stemp * temp + ctemp * a[i*lda];
                        }
                    }
                }
            }
        } else if (lsame_reimpl(pivot, 'B')) {
            if (lsame_reimpl(direct, 'F')) {
                for (j = 0; j < n - 1; ++j) {
                    ctemp = c[j];
                    stemp = s[j];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < m; ++i) {
                            temp = a[i*lda + j];
                            a[i*lda + j] = stemp * a[i*lda + (n-1)] + ctemp * temp;
                            a[i*lda + (n-1)] = ctemp * a[i*lda + (n-1)] - stemp * temp;
                        }
                    }
                }
            } else {
                for (j = n - 2; j >= 0; --j) {
                    ctemp = c[j];
                    stemp = s[j];
                    if (ctemp != ONE || stemp != ZERO) {
                        for (i = 0; i < m; ++i) {
                            temp = a[i*lda + j];
                            a[i*lda + j] = stemp * a[i*lda + (n-1)] + ctemp * temp;
                            a[i*lda + (n-1)] = ctemp * a[i*lda + (n-1)] - stemp * temp;
                        }
                    }
                }
            }
        }
    }
}