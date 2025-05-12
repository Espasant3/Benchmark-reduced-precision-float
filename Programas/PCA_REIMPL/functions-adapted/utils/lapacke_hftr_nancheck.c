
#include "../include/lapacke_utils_reimpl.h" //Se puede evitar poner la ruta completa si se configura el compilador con algo como gcc -I./include -o programa utils/lapacke_ssy_nancheck_reimpl.c

/* Check a matrix for NaN entries. */

lapack_logical LAPACKE_hftr_nancheck( int matrix_layout, char uplo, char diag,
                                      lapack_int n,
                                      const _Float16 *a,
                                      lapack_int lda )
{
    lapack_int i, j, st;
    lapack_logical colmaj, lower, unit;

    if( a == NULL ) return (lapack_logical) 0;

    colmaj = ( matrix_layout == LAPACK_COL_MAJOR );
    lower  = lsame_reimpl( uplo, 'l' );
    unit   = lsame_reimpl( diag, 'u' );

    if( ( !colmaj && ( matrix_layout != LAPACK_ROW_MAJOR ) ) ||
        ( !lower  && !lsame_reimpl( uplo, 'u' ) ) ||
        ( !unit   && !lsame_reimpl( diag, 'n' ) ) ) {
        /* Just exit if any of input parameters are wrong */
        return (lapack_logical) 0;
    }
    if( unit ) {
        /* If unit, then don't touch diagonal, start from 1st column or row */
        st = 1;
    } else  {
        /* If non-unit, then check diagonal also, starting from [0,0] */
        st = 0;
    }

    /* Since col_major upper and row_major lower are equal,
     * and col_major lower and row_major upper are equals too -
     * using one code for equal cases. XOR( colmaj, upper )
     */
    if( ( colmaj || lower ) && !( colmaj && lower ) ) {
        for( j = st; j < n; j++ ) {
            for( i = 0; i < MIN( j+1-st, lda ); i++ ) {
                if( LAPACK_HFISNAN( a[i+j*lda] ) )
                    return (lapack_logical) 1;
            }
        }
    } else {
        for( j = 0; j < n-st; j++ ) {
            for( i = j+st; i < MIN( n, lda ); i++ ) {
                if( LAPACK_HFISNAN( a[i+j*lda] ) )
                    return (lapack_logical) 1;
            }
        }
    }
    return (lapack_logical) 0;
}
