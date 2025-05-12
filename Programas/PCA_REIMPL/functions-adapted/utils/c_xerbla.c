
#include "../include/lapacke_utils_reimpl.h"

/**
 * \file c_xerbla.c
 * \brief Implementación de la función xerbla de BLAS
 */

/**
 * \brief Maneja errores de parámetros inválidos en estilo Netlib/LAPACK
 * 
 * \details Versión modificada del manejador de errores que:
 *          - Recorta espacios finales del nombre de la rutina
 *          *\*Diferencias clave vs LAPACKE_xerbla*\*:
 *              1. Termina la ejecución del programa con EXIT_FAILURE
 *              2. Imprime siempre en stderr (vs stdout de LAPACKE_xerbla)
 *              3. Mensaje específico para parámetros ilegales únicamente
 *              4. Formato de mensaje idéntico a la implementación original de Netlib
 *
 * \param[in] srname Nombre de la rutina LAPACK (con posible padding de espacios)
 * \param[in] info Número del parámetro inválido (debe ser > 0)
 *
 * \return void (la función no retorna - finaliza la ejecución)
 *
 * \note El recorte de espacios se hace para mantener compatibilidad con implementaciones históricas
 * \warning Esta función llama a exit() - cualquier limpieza posterior no se ejecutará
 * \warning El parámetro info debe ser positivo (comportamiento indefinido para valores <= 0)
 *
 * \example
 * c_xerbla("DPOTRF ", 5); // Imprime: " ** On entry to DPOTRF parameter number 5 had an illegal value"
 *                         // y termina el programa con código EXIT_FAILURE
 *
 * \see LAPACKE_xerbla Para versión que no finaliza la ejecución y maneja más tipos de errores
 */

void c_xerbla(const char *srname, int info) {
    size_t len = strlen(srname);
    while (len > 0 && srname[len - 1] == ' ') {
        len--;
    }

    // Mensaje de error idéntico al original
    fprintf(stderr, " ** On entry to %.*s parameter number %d had an illegal value\n",
            (int)len, srname, info);

    exit(EXIT_FAILURE);
}