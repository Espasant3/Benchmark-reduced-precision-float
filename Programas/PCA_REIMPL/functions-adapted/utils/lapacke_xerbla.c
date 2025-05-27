
#include "lapacke_utils_reimpl.h"

/**
 * \file lapacke_xerbla.c
 * \brief Implementación de la función LAPACKE_xerbla
 */

/**
 * \brief Función de manejo de errores para rutinas LAPACKE
 * 
 * \details Imprime mensajes de error específicos basados en códigos de error de LAPACKE:
 *          - Memoria insuficiente para arrays de trabajo
 *          - Memoria insuficiente para transposición de matrices
 *          - Parámetros incorrectos en funciones
 *
 * \param[in] name Nombre de la función LAPACKE donde ocurrió el error
 * \param[in] info Código de error:
 *                 - LAPACK_WORK_MEMORY_ERROR: Error de memoria para array de trabajo
 *                 - LAPACK_TRANSPOSE_MEMORY_ERROR: Error de memoria para transpuesta
 *                 - Valores negativos: Número de parámetro inválido
 *
 * \return void
 *
 * \note Los mensajes de error se imprimen directamente a stdout.
 * \warning Esta función es crítica para el manejo de errores en rutinas LAPACKE. 
 *          Modificarla puede afectar el comportamiento de toda la biblioteca.
 *
 * \example
 * LAPACKE_xerbla("ssyev", -5); // Imprime: "Wrong parameter 5 in ssyev"
 */

void LAPACKE_xerbla( const char *name, lapack_int info )
{
    if( info == LAPACK_WORK_MEMORY_ERROR ) {
        printf( "Not enough memory to allocate work array in %s\n", name );
    } else if( info == LAPACK_TRANSPOSE_MEMORY_ERROR ) {
        printf( "Not enough memory to transpose matrix in %s\n", name );
    } else if( info < 0 ) {
        printf( "Wrong parameter %d in %s\n", -(int) info, name );
    }
}
