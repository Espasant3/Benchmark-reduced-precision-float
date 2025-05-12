
#include "../include/lapacke_c_interfaces.h"

lapack_int LAPACKE_hfsterf_work( lapack_int n, _Float16* d, _Float16* e )
{
    lapack_int info = 0;
    /* Call LAPACK function and adjust info */
    hfsterf( n, d, e, &info );
    return info;
}
