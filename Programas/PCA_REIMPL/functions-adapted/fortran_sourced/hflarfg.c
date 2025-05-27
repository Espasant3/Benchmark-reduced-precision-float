
#include "lapacke_utils_reimpl.h"

/**
 * \file hflarfg.c
 * \brief Genera un reflector de Householder en lapack_float
 */

/**
 * \brief Genera un reflector elemental de Householder en precisión media
 * 
 * \details Versión lapack_float de LAPACK's SLARFG. Calcula un reflector H tal que:
 *          \f[
 *          H \cdot \begin{pmatrix} \alpha \\ x \end{pmatrix} = \begin{pmatrix} \beta \\ 0 \end{pmatrix}
 *          \f]
 *          donde H = I - \tau \cdot v \cdot v^T y v[0] = 1.
 * 
 * \param[in] n     Orden del reflector (n >= 0)
 * \param[in,out] alpha Escalar de entrada/salida:
 *                  - Entrada: Valor original \alpha
 *                  - Salida: Valor \beta resultante
 * \param[in,out] x Array lapack_float de dimensión (1 + (n-2)*|incx|)
 *                  - Entrada: Vector a reflejar (elementos x[0] a x[(n-2)*incx])
 *                  - Salida: Vector v del reflector (v[0] = 1 implícito)
 * \param[in] incx  Incremento entre elementos de x (incx != 0)
 * \param[out] tau  Escalar del reflector: \tau = (beta - alpha)/beta
 * 
 * \note
 * - Implementación numéricamente estable con escalado para evitar underflow
 * - Límite máximo de 20 iteraciones en bucle de escalado
 * - v se almacena en x sin el primer elemento (implícito 1)
 * 
 * \warning
 * - No verifica valores NaN/Inf en alpha o x
 * - incx=0 causa comportamiento indefinido
 * - La precisión reducida de lapack_float puede afectar ortogonalidad
 * 
 * \par Algoritmo clave:
 * 1. Cálculo de norma: xnorm = ||x||_2
 * 2. beta = -sign(alpha) * sqrt(alpha² + xnorm²)
 * 3. Manejo de underflow con escalado iterativo (safmin = FP16_TRUE_MIN/FP16_EPSILON)
 * 4. Cálculo de tau y actualización de x
 * 
 * \example
 * lapack_float alpha = 3.0F16;
 * lapack_float x[3] = {1.0, 2.0, 3.0};
 * lapack_float tau;
 * 
 * hflarfg(4, &alpha, x, 1, &tau); 
 * // alpha ahora contiene beta, x almacena v[1..3]
 * 
 * \see hflarf   Para aplicación del reflector
 * \see hfnrm2   Para cálculo de norma en lapack_float
 * \see hflamch_half_precision  Para obtención de constantes máquina
 */

void hflarfg(int n, lapack_float *alpha, lapack_float *x, lapack_int incx, lapack_float *tau) {
    lapack_float beta, xnorm, safmin, rsafmn;
    int knt = 0;
    const lapack_float ZERO = (lapack_float) 0.0;
    const lapack_float ONE = (lapack_float) 1.0;

    if (n <= 1) {
        *tau = ZERO;
        return;
    }

    xnorm = hfnrm2(n - 1, x, incx);

    if (xnorm == ZERO) {
        *tau = ZERO;
    } else {

        beta = (lapack_float)( -copysignf((float) hflapy2( *alpha, xnorm ), (float)(*alpha) ));

        safmin = hflamch_half_precision('S') / hflamch_half_precision('E');


        if (ABS_half_precision(beta) < safmin) {
            rsafmn = ONE / safmin;
            do {
                knt++;
                hfscal(n - 1, rsafmn, x, incx);
                beta *= rsafmn;
                *alpha *= rsafmn;
            } while (ABS_half_precision(beta) < safmin && knt < 20);
            xnorm = hfnrm2(n - 1, x, incx);
            beta = (lapack_float)( -copysignf((float) hflapy2( *alpha, xnorm ), (float)(*alpha) ));
        }

        *tau = (beta - *alpha) / beta;
        hfscal(n - 1, ONE / (*alpha - beta), x, incx);

        for (int j = 0; j < knt; j++) {
            beta *= safmin;
        }
        *alpha = beta;
    }
}