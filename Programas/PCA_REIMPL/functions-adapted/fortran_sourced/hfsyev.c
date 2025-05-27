
#include "lapacke_utils_reimpl.h" 

void hfsyev(char jobz, char uplo, lapack_int n, lapack_float* a, lapack_int lda, lapack_float* w, lapack_float* work, lapack_int lwork, lapack_int* info) {

    // Constantes y variables
    const lapack_float ZERO = (lapack_float)0.0;
    const lapack_float ONE = (lapack_float)1.0;
    const lapack_float TWO = (lapack_float)2.0;

    // Flags lógicos
    lapack_int wantz = lsame_reimpl(jobz, 'V');
    lapack_int lower = lsame_reimpl(uplo, 'L');
    lapack_int lquery = (lwork == -1);

    lapack_int lwkopt = 0; // Tamaño óptimo del workspace

    *info = 0;

    // ========= Validación de parámetros =========
    if (!wantz && !lsame_reimpl(jobz, 'N')) { // if (!(wantz || lsame_reimpl(jobz, 'N')))
        *info = -1;
    } else if (!lower && !lsame_reimpl(uplo, 'U')) {
        *info = -2;
    } else if (n < 0) {
        *info = -3;
    } else if (lda < MAX(1, n)) {
        *info = -5;
    }
    
    if(*info == 0) {
        // ========= Cálculo del workspace óptimo =========

        lapack_int nb = ilaenv_reimpl_half_precision(1, "SSYTRD", "U", n-1, -1, -1, -1); // En principio tiene que seguir con "SSYTRD" hasta que se adapte el programa)
        // Tecnicamente seria posible duplicar este valor si la arquitectura soportase el uso de float de media precisión
        lwkopt = MAX(1, (nb + 2) * n);
        work[0] = hfroundup_lwork(lwkopt);

        if (lwork < MAX(1, 3 * n - 1) && !lquery) {
            *info = -8;
        }
    }
    
    if (*info != 0) {
        LAPACKE_xerbla("HFSYEV", -(*info));
        return;
    } else if (lquery) {
        return;  // Solo consulta de workspace
    }

    // ======== Casos triviales (N=0 o N=1) ========
    if (n == 0) { // Quick return if possible
        return;
    }

    if (n == 1) {
        w[0] = a[0];
        work[0] = TWO;
        if (wantz) {
            a[0] = ONE;
        }
        return;
    }
    
    // Get machine constants.

    lapack_float safmin = hflamch_half_precision('S'); 
    lapack_float eps = hflamch_half_precision('E');
    lapack_float smlnum = safmin / eps;
    lapack_float bignum = ONE / smlnum;
    lapack_float rmin = custom_sqrtf_half_precision(smlnum);
    lapack_float rmax = custom_sqrtf_half_precision(bignum);

    // Scale matrix to allowable range, if necessary.

    lapack_float anrm = hflansy('M', uplo, n, a, lda, work);
    int iscale = 0;
    lapack_float sigma = ONE;

    if (anrm > ZERO && anrm < rmin) {
        iscale = 1;
        sigma = rmin / anrm;
    } else if (anrm > rmax) {
        iscale = 1;
        sigma = rmax / anrm;
    }

    if (iscale == 1) {
        hflascl(uplo, 0, 0, ONE, sigma, n, n, a, lda, info);
        if (*info != 0) {
            return;
        }
    }

    // ======== Reducción a forma tridiagonal (SSYTRD) ========
    lapack_int inde = 0;
    lapack_int indtau = inde + n;
    lapack_int indwrk = indtau + n;
    lapack_int llwork = lwork - indwrk;

    hfsytrd(uplo, n, a, lda, w, &work[inde], &work[indtau], work, lwork, info);
    
    if (*info != 0) {
        return;
    }
    // ======== Cálculo de valores/vectores propios ========
    if (!wantz) {
        hfsterf(n, w, &work[inde], info);
    } else {
        // Generar matriz ortogonal (SORGTR)
        hforgtr(uplo, n, a, lda, &work[indtau], &work[indwrk], llwork, info);
        
        if (*info != 0) {
            return;
        }

        // Calcular vectores propios (SSTEQR)
        hfsteqr('V', n, w, &work[inde], a, lda, &work[indtau], info);

        if (*info != 0) {
            return;
        }
    }
    // ======== Re-escalado de valores propios ========
    if (iscale == 1) {
        lapack_int imax = (*info == 0) ? n - 1 : *info - 1;
        hfscal(imax, ONE / sigma, w, 1);
    }

    // ======== Devolver tamaño óptimo del workspace ========
    work[0] = hfroundup_lwork(lwkopt);
}