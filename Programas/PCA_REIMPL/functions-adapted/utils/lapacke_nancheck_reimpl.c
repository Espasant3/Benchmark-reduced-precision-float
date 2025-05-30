/*****************************************************************************
  Copyright (c) 2017, Intel Corp.
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
*****************************************************************************
* Contents: Native C interface to control NaN checking
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
 * \file lapacke_nancheck_reimpl.c
 * \brief Implementación de chequeo de NaN para LAPACKE con control dinámico
 * 
 * \note El include puede configurarse con flags de compilación:
 * \code
 * gcc -I./include -o programa utils/lapacke_ssy_nancheck_reimpl.c
 * \endcode
 */

/**
 * \defgroup NANCHECK_CONTROL Control de verificación de NaN
 * \brief Sistema de activación/desactivación dinámica de chequeos NaN
 * 
 * Mecanismo híbrido que combina:
 * - Variable de entorno: LAPACKE_NANCHECK
 * - API programática: LAPACKE_set_nancheck_reimpl()
 * - Valor por defecto: Activado (1)
 */

/**
 * \brief Variable interna de estado para chequeo NaN
 * 
 * \details Almacena el estado actual siguiendo esta lógica:
 * - -1: No inicializado (primera lectura consultará entorno)
 * - 0: Desactivado
 * - 1: Activado
 * 
 * \warning Modificación directa no soportada - usar LAPACKE_set_nancheck_reimpl()
 */
static int nancheck_flag = -1;

/**
 * \brief Establece el modo de verificación NaN manualmente
 * 
 * \param[in] flag Estado deseado:
 *               - 0: Desactiva chequeos NaN
 *               - Cualquier otro valor: Activa chequeos NaN
 * 
 * \ingroup NANCHECK_CONTROL
 * 
 * \example
 * LAPACKE_set_nancheck_reimpl(0); // Deshabilita globalmente
 */

void LAPACKE_set_nancheck_reimpl( int flag )
{
    nancheck_flag = ( flag ) ? 1 : 0;
}

/**
 * \brief Obtiene el estado actual de verificación NaN
 * 
 * \return int Estado actualizado:
 *           - 0: Verificación desactivada
 *           - 1: Verificación activada
 * 
 * \details Lógica de obtención:
 * 1. Si ya fue establecido manualmente: Retorna valor configurado
 * 2. Si no: Consulta variable de entorno LAPACKE_NANCHECK
 * 3. Si no existe variable: Usa valor por defecto (1)
 * 
 * \note Solo consulta la variable de entorno una vez durante la ejecución
 * 
 * \ingroup NANCHECK_CONTROL
 * 
 * \example
 * int check = LAPACKE_get_nancheck_reimpl();
 * if(check) {
 *     // Realizar chequeos NaN
 * }
 */

int LAPACKE_get_nancheck_reimpl( )
{
    char* env;
    if ( nancheck_flag != -1 ) {
        return nancheck_flag;
    }

    /* Check environment variable, once and only once */
    env = getenv( "LAPACKE_NANCHECK" );
    if ( !env ) {
        /* By default, NaN checking is enabled */
        nancheck_flag = 1;
    } else {
        nancheck_flag = atoi( env ) ? 1 : 0;
    }

    return nancheck_flag;
}
