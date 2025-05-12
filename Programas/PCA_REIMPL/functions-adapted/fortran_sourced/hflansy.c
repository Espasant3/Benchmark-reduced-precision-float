

#include "../include/lapacke_utils_reimpl.h"

// Main function to compute matrix norms for real symmetric matrices
_Float16 hflansy(char norm, char uplo, int n, const _Float16 *a, int lda, _Float16 *work) {
    _Float16 value = 0.0F16;

    if (n == 0) return value;

    if (lsame_reimpl(norm, 'M')) {
        // Maximum absolute value
        if (lsame_reimpl(uplo, 'U')) {
            for (int j = 0; j < n; ++j) {
                for (int i = 0; i <= j; ++i) {
                    _Float16 current = ABS_Float16(a[i + j * lda]);
                    if (current > value || LAPACK_HFISNAN(current)) value = current;
                }
            }
        } else {
            for (int j = 0; j < n; ++j) {
                for (int i = j; i < n; ++i) {
                    _Float16 current = ABS_Float16(a[i + j * lda]);
                    if (current > value || LAPACK_HFISNAN(current)) value = current;
                }
            }
        }
    } else if (lsame_reimpl(norm, 'I') || lsame_reimpl(norm, 'O') || norm == '1') {
        // Infinity norm or 1-norm
        value = 0.0F16;
        if (lsame_reimpl(uplo, 'U')) {
            for (int i = 0; i < n; ++i) work[i] = 0.0F16;
            for (int j = 0; j < n; ++j) {
                _Float16 sum = 0.0F16;
                for (int i = 0; i < j; ++i) {
                    _Float16 absa = ABS_Float16(a[i + j * lda]);
                    sum += absa;
                    work[i] += absa;
                }
                sum += ABS_Float16(a[j + j * lda]);
                work[j] += sum;
            }
            for (int i = 0; i < n; ++i) {
                _Float16 current = work[i];
                if (current > value || LAPACK_HFISNAN(current)) value = current;
            }
        } else {
            for (int i = 0; i < n; ++i) work[i] = 0.0F16;
            for (int j = 0; j < n; ++j) {
                _Float16 sum = work[j] + ABS_Float16(a[j + j * lda]);
                for (int i = j + 1; i < n; ++i) {
                    _Float16 absa = ABS_Float16(a[i + j * lda]);
                    sum += absa;
                    work[i] += absa;
                }
                if (sum > value || LAPACK_HFISNAN(sum)) value = sum;
            }
        }
    } else if (lsame_reimpl(norm, 'F') || lsame_reimpl(norm, 'E')) {
        // Frobenius norm
        _Float16 scale = 0.0F16;
        _Float16 sum = 1.0F16;
        if (lsame_reimpl(uplo, 'U')) {
            for (int j = 1; j < n; ++j) {
                //LAPACKE_hflassq(j, &a[j * lda], 1, &scale, &sum);
                hflassq(j, (const _Float16*) &a[j * lda], 1, &scale, &sum);
            }
        } else {
            for (int j = 0; j < n - 1; ++j) {
                int num_elements = n - j - 1;
                //LAPACKE_hflassq(num_elements, &a[(j + 1) + j * lda], 1, &scale, &sum);
                hflassq(num_elements, (const _Float16*) &a[(j + 1) + j * lda], 1, &scale, &sum);
            }
        }
        sum *= 2.0F16;
        //LAPACKE_hflassq(n, a, lda + 1, &scale, &sum);
        hflassq(n, (const _Float16*) a, lda + 1, &scale, &sum);
        value = scale * sqrtf(sum);
    } else {
        // Norma no soportada
        return 0.0F16;
    }

    return value;
}