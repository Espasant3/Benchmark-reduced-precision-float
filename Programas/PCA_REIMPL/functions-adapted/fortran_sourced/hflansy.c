

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
        _Float16 scale = 0.0F16;
        _Float16 sum = 1.0F16;
        bool has_nan = false;

        // Verificar NaN en todos los elementos relevantes de la matriz
        // Dentro del bloque que verifica NaN en hflansy:
        if (lsame_reimpl(uplo, 'U')) {
            for (int j = 0; j < n; ++j) {
                for (int i = 0; i <= j; ++i) {
                    _Float16 elem = a[i + j * lda];
                    if (LAPACK_HFISNAN(elem)) {
                        has_nan = true;
                        break;
                    }
                }
                if (has_nan) break;
            }
        } else {
            for (int j = 0; j < n; ++j) {
                for (int i = j; i < n; ++i) {
                    if (LAPACK_HFISNAN(a[i + j * lda])) {
                        has_nan = true;
                        break;
                    }
                }
                if (has_nan) break;
            }
        }

        if (has_nan) {
            return (_Float16)NAN;  // Retornar NaN inmediatamente
        }

        // Si no hay NaN, calcular la norma
        if (lsame_reimpl(uplo, 'U')) {
            for (int j = 0; j < n; ++j) {
                hflassq(j + 1, &a[j], lda, &scale, &sum);
            }
        } else {
            for (int j = 0; j < n; ++j) {
                hflassq(n - j, &a[j * lda + j], lda, &scale, &sum);
            }
        }

        value = scale * custom_sqrtf16(sum);

        if (!has_nan) {
            // Aplicar saturación solo si no hay NaN
            if (value > FP16_MAX) {
                value = FP16_MAX;
            }
        }

    }

    return value;
}

