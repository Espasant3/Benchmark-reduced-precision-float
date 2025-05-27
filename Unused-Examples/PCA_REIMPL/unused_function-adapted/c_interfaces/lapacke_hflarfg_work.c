
#include "../include/lapacke_c_interfaces.h" 

lapack_int LAPACKE_hflarfg_work( lapack_int n, _Float16* alpha, _Float16* x,
                                lapack_int incx, _Float16* tau )
{
    lapack_int info = 0;
    /* Call LAPACK function and adjust info */
    hflarfg( n, alpha, x, incx, tau );
    return info;
}
