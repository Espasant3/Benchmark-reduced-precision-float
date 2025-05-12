#include "../include/lapacke_utils_reimpl.h" //Se puede evitar poner la ruta completa si se configura el compilador con algo como gcc -I./include -o programa utils/lapacke_ssyev_reimpl.c

/* Check a matrix for NaN entries. */

lapack_logical LAPACKE_hfge_nancheck( int matrix_layout, lapack_int m,
                                      lapack_int n,
                                      const _Float16 *a,
                                      lapack_int lda )
{
    lapack_int i, j;

    if( a == NULL ) return (lapack_logical) 0;

    if( matrix_layout == LAPACK_COL_MAJOR ) {
        for( j = 0; j < n; j++ ) {
            for( i = 0; i < MIN( m, lda ); i++ ) {
                if( LAPACK_HFISNAN( a[i+(size_t)j*lda] ) )
                    return (lapack_logical) 1;
            }
        }
    } else if ( matrix_layout == LAPACK_ROW_MAJOR ) {
        for( i = 0; i < m; i++ ) {
            for( j = 0; j < MIN( n, lda ); j++ ) {
                if( LAPACK_HFISNAN( a[(size_t)i*lda+j] ) )
                    return (lapack_logical) 1;
            }
        }
    }
    return (lapack_logical) 0;
}
