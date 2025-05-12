
#include "../include/lapacke_utils_reimpl.h"

void hflacpy(char uplo, int m, int n, const _Float16 *a, int lda, _Float16 *b, int ldb) {
    int i, j;
    
    if (lsame_reimpl(uplo, 'U')) {
        for (j = 0; j < n; ++j) {
            const int max_i = (j + 1 < m) ? j + 1 : m;
            for (i = 0; i < max_i; ++i) {
                b[i + j * ldb] = a[i + j * lda];
            }
        }
    } else if (lsame_reimpl(uplo, 'L')) {
        for (j = 0; j < n; ++j) {
            const int start_i = (j < m) ? j : m;
            for (i = start_i; i < m; ++i) {
                b[i + j * ldb] = a[i + j * lda];
            }
        }
    } else {
        for (j = 0; j < n; ++j) {
            for (i = 0; i < m; ++i) {
                b[i + j * ldb] = a[i + j * lda];
            }
        }
    }
}