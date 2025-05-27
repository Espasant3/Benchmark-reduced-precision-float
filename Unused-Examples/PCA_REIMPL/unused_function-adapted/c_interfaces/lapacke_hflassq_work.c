
#include "../include/lapacke_c_interfaces.h" //Se puede evitar poner la ruta completa si se configura el compilador con algo como gcc -I./include -o programa utils/lapacke_ssy_nancheck_reimpl.c

lapack_int LAPACKE_hflassq_work( lapack_int n, _Float16* x, lapack_int incx, _Float16* scale, _Float16* sumsq )
{
    lapack_int info = 0;
    hflassq( n, x, incx, scale, sumsq );
    return info;
}
