
#include "../include/lapacke_utils_reimpl.h" //Se puede evitar poner la ruta completa si se configura el compilador con algo como gcc -I./include -o programa utils/lapacke_slascl_reimpl.c

void hfsyev(char jobz, char uplo, lapack_int n, _Float16* a, lapack_int lda, _Float16* w, _Float16* work, lapack_int lwork, lapack_int* info) {

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

        lapack_int nb = ilaenv_reimpl_Float16(1, "SSYTRD", "U", n-1, -1, -1, -1); // En principio tiene que seguir con "SSYTRD" hasta que se adapte el programa)
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
        work[0] = 2.0F16;
        if (wantz) {
            a[0] = 1.0F16;
        }
        return;
    }
    
    // Get machine constants.

    _Float16 safmin = hflamch_Float16('S'); 
    _Float16 eps = hflamch_Float16('E');
    _Float16 smlnum = safmin / eps;
    _Float16 bignum = 1.0F16 / smlnum;
    _Float16 rmin = custom_sqrtf16(smlnum);
    _Float16 rmax = custom_sqrtf16(bignum);

    // Scale matrix to allowable range, if necessary.

    _Float16 anrm = hflansy('M', uplo, n, a, lda, work);
    int iscale = 0;
    _Float16 sigma = 1.0F16;

    if (anrm > 0.0F16 && anrm < rmin) {
        iscale = 1;
        sigma = rmin / anrm;
    } else if (anrm > rmax) {
        iscale = 1;
        sigma = rmax / anrm;
    }

    if (iscale == 1) {
        //LAPACKE_hflascl(LAPACK_COL_MAJOR, uplo, 0, 0, 1.0F16, sigma, n, n, a, lda);
        hflascl(uplo, 0, 0, 1.0F16, sigma, n, n, a, lda, info);
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
        hfscal(imax, 1.0F16 / sigma, w, 1);
    }

    // ======== Devolver tamaño óptimo del workspace ========
    work[0] = hfroundup_lwork(lwkopt);
}