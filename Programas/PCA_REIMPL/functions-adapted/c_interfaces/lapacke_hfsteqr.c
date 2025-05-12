/*****************************************************************************
  Copyright (c) 2014, Intel Corp.
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
* Contents: Native high-level C interface to LAPACK function ssteqr
* Author: Intel Corporation
*****************************************************************************/

#include "../include/lapacke_c_interfaces.h" //Se puede evitar poner la ruta completa si se configura el compilador con algo como gcc -I./include -o programa utils/lapacke_ssy_nancheck_reimpl.c

lapack_int LAPACKE_hfsteqr( int matrix_layout, char compz, lapack_int n, _Float16* d,
                           _Float16* e, _Float16* z, lapack_int ldz )
{
    lapack_int info = 0;
    /* Additional scalars declarations for work arrays */
    lapack_int lwork;
    _Float16* work = NULL;
    if( matrix_layout != LAPACK_COL_MAJOR && matrix_layout != LAPACK_ROW_MAJOR ) {
        LAPACKE_xerbla( "LAPACKE_hfsteqr", -1 );
        return -1;
    }
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck_reimpl() ) {
        /* Optionally check input matrices for NaNs */
        if( LAPACKE_hf_nancheck( n, d, 1 ) ) {
            return -4;
        }
        if( LAPACKE_hf_nancheck( n-1, e, 1 ) ) {
            return -5;
        }
        if( lsame_reimpl( compz, 'v' ) ) {
            if( LAPACKE_hfge_nancheck( matrix_layout, n, n, z, ldz ) ) {
                return -6;
            }
        }
    }
#endif
    /* Additional scalars initializations for work arrays */
    if( lsame_reimpl( compz, 'n' ) ) {
        lwork = 1;
    } else {
        lwork = MAX(1,2*n-2);
    }
    /* Allocate memory for working array(s) */
    work = (_Float16*)LAPACKE_malloc( sizeof(_Float16) * lwork );
    if( work == NULL ) {
        info = LAPACK_WORK_MEMORY_ERROR;
        goto exit_level_0;
    }
    /* Call middle-level interface */
    info = LAPACKE_hfsteqr_work( matrix_layout, compz, n, d, e, z, ldz, work );
    /* Release memory and exit */
    LAPACKE_free( work );
exit_level_0:
    if( info == LAPACK_WORK_MEMORY_ERROR ) {
        LAPACKE_xerbla( "LAPACKE_hfsteqr", info );
    }
    return info;
}
