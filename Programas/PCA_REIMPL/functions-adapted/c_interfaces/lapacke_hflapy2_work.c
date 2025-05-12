
#include "../include/lapacke_c_interfaces.h"

_Float16 LAPACKE_hflapy2_work( _Float16 x, _Float16 y )
{
    float res = 0.;
    /* Call LAPACK function and adjust info */
    res = hflapy2( x, y );
    return res;
}
