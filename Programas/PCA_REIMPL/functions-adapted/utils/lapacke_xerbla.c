/*****************************************************************************
  Copyright (c) 2010, Intel Corp.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************
* Contents: Native C interface to LAPACK lsame
* Author: Intel Corporation
*****************************************************************************/

/*****************************************************************************
 * Copyright original:
 *   Copyright (c) 1992-2025 The University of Tennessee [...]  
 *   Copyright (c) 2000-2025 The University of California Berkeley [...]  
 *   Copyright (c) 2006-2025 The University of Colorado Denver [...]  
 * 
 * Modificaciones (c) 2025 Eloi Barcón Piñeiro
 * 
 * Licencia: BSD modificada (ver ../../../../LICENSE_LAPACK)
 *****************************************************************************/

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
