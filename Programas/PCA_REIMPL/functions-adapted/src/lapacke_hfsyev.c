
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
* Contents: Native high-level C interface to LAPACK function ssyev
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

lapack_int LAPACKE_hfsyev( int matrix_layout, char jobz, char uplo, lapack_int n,
                          lapack_float* a, lapack_int lda, lapack_float* w )
{
    lapack_int info = 0;
    lapack_int lwork = -1;
    lapack_float* work = NULL;
    lapack_float work_query;
    if( matrix_layout != LAPACK_COL_MAJOR && matrix_layout != LAPACK_ROW_MAJOR ) {
        LAPACKE_xerbla( "LAPACKE_hfsyev", -1 );
        return -1;
    }
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck_reimpl() ) {
        /* Optionally check input matrices for NaNs */
        if( LAPACKE_hfsy_nancheck( matrix_layout, uplo, n, a, lda ) ) {
            return -5;
        }
    }
#endif
    /* Query optimal working array(s) size */
    info = LAPACKE_hfsyev_work( matrix_layout, jobz, uplo, n, a, lda, w,
                               &work_query, lwork );
    if( info != 0 ) {
        goto exit_level_0;
    }
    lwork = (n < 130) ? (lapack_int)work_query : 3 * n - 1; // Use a larger work size for larger matrices
    /* Allocate memory for work arrays */
    work = (lapack_float*)LAPACKE_calloc(lwork ,sizeof(lapack_float));
    if( work == NULL ) {
        info = LAPACK_WORK_MEMORY_ERROR;
        goto exit_level_0;
    }
    /* Call middle-level interface */
    info = LAPACKE_hfsyev_work( matrix_layout, jobz, uplo, n, a, lda, w, work,
                               lwork );
    /* Release memory and exit */
    LAPACKE_free( work );
exit_level_0:
    if( info == LAPACK_WORK_MEMORY_ERROR ) {
        LAPACKE_xerbla( "LAPACKE_hfsyev", info );
    }
    return info;
}
