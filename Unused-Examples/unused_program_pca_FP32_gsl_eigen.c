#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_eigen.h>

// Estructura para representar una matriz
typedef struct {
    int rows;
    int cols;
    float** data;
} Matrix;

// Función para crear una estructura Matrix de tamaño rows x cols
Matrix* _create_Matrix(int rows, int cols) {
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = malloc(rows * sizeof(float *));
    for(int i = 0; i < rows; i++) {
        matrix->data[i] = malloc(cols * sizeof(float));
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            matrix->data[i][j] = 0;
        }
    }

    return matrix;
}

// Función para liberar la memoria de una estructura Matrix
void _free_matrix(Matrix* matrix) {
    for(int i = 0; i < matrix->rows; i++) {
        free(matrix->data[i]);
    }
    free(matrix->data);
    free(matrix);
}

// Función para imprimir una matriz
void _print_matrix(Matrix* matrix) {
    for(int i = 0; i < matrix->rows; i++) {
        printf("\t");
        for(int j = 0; j < matrix->cols; j++) {
            printf("%f\t", matrix->data[i][j]);
        }
        printf("\n");
    }
}

// Función para copiar una matriz a otra
void _copy_matrix(Matrix* source, Matrix* destination) {
    for(int i = 0; i < source->rows; i++) {
        for(int j = 0; j < source->cols; j++) {
            destination->data[i][j] = source->data[i][j];
        }
    }
}

// Función para calcular las medias y desviaciones estándar de cada columna de la matriz
void _calc_means_and_deviations(Matrix* matrix, float *medias, float *desviaciones) {
    for(int j = 0; j < matrix->cols; j++) {
        float suma = 0;
        for(int i = 0; i < matrix->rows; i++) {
            suma += matrix->data[i][j];
        }
        medias[j] = suma / matrix->rows;

        float suma_desviaciones = 0;
        for(int i = 0; i < matrix->rows; i++) {
            suma_desviaciones += (matrix->data[i][j] - medias[j]) * (matrix->data[i][j] - medias[j]);
        }
        desviaciones[j] = sqrt(suma_desviaciones / matrix->rows);
    }
}

// Función para estandarizar la matriz
void standarize_matrix(Matrix* matrix) {
    float *medias = malloc(matrix->cols * sizeof(float));
    float *desviaciones = malloc(matrix->cols * sizeof(float));
    _calc_means_and_deviations(matrix, medias, desviaciones);

    for(int i = 0; i < matrix->rows; i++) {
        for(int j = 0; j < matrix->cols; j++) {
            matrix->data[i][j] = (matrix->data[i][j] - medias[j]) / desviaciones[j];
        }
    }
    free(medias);
    free(desviaciones);
}

// Función para calcular la matriz de covarianza
void calculate_covariance(Matrix* matrix, Matrix* covariance) {
    for(int i = 0; i < matrix->cols; i++) {
        for(int j = 0; j < matrix->cols; j++) {
            double suma = 0.0;
            for(int k = 0; k < matrix->rows; k++) {
                suma += (matrix->data[k][i] * matrix->data[k][j]);
            }
            covariance->data[i][j] = suma / (matrix->rows - 1);
        }
    }
}

// Función para calcular los valores y vectores propios de la matriz de covarianza
void calculate_eigenvalues_and_eigenvectors(Matrix* covariance, gsl_vector *eigenvalues, gsl_matrix *eigenvectors) {
    gsl_matrix *covariance_gsl = gsl_matrix_alloc(covariance->rows, covariance->cols);
    for(int i = 0; i < covariance->rows; i++) {
        for(int j = 0; j < covariance->cols; j++) {
            gsl_matrix_set(covariance_gsl, i, j, (double)covariance->data[i][j]);
        }
    }
    gsl_eigen_symmv_workspace *workspace = gsl_eigen_symmv_alloc(covariance->rows);
    if (workspace == NULL) {
        fprintf(stderr, "Error allocating workspace for eigenvalue computation.\n");
        gsl_matrix_free(covariance_gsl);
        return;
    }

    int status = gsl_eigen_symmv(covariance_gsl, eigenvalues, eigenvectors, workspace);
    if (status != 0) {
        fprintf(stderr, "Error computing eigenvalues and eigenvectors: %s\n", gsl_strerror(status));
        gsl_eigen_symmv_free(workspace);
        gsl_matrix_free(covariance_gsl);
        return;
    }

    gsl_eigen_symmv_free(workspace);
    gsl_eigen_symmv_sort(eigenvalues, eigenvectors, GSL_EIGEN_SORT_VAL_DESC);
    gsl_matrix_free(covariance_gsl);
}

// Función para transformar los datos usando los vectores propios
void transform_data(Matrix* matrix, gsl_matrix *eigenvectors, Matrix* transformed_data) {
    for(int i = 0; i < matrix->rows; i++) {
        for(int j = 0; j < matrix->cols; j++) {
            transformed_data->data[i][j] = 0;
            for(int l = 0; l < matrix->cols; l++) {
                transformed_data->data[i][j] += (double)matrix->data[i][l] * gsl_matrix_get(eigenvectors, l, j);
            }
        }
    }
}

// Función principal para realizar PCA
void do_pca(Matrix* matrix) {
    standarize_matrix(matrix);

    Matrix* covariance = _create_Matrix(matrix->cols, matrix->cols);
    calculate_covariance(matrix, covariance);

    gsl_vector *eigenvalues = gsl_vector_alloc(covariance->rows);
    gsl_matrix *eigenvectors = gsl_matrix_alloc(covariance->rows, covariance->cols);

    calculate_eigenvalues_and_eigenvectors(covariance, eigenvalues, eigenvectors);

    printf("Valores propios: \n");
    for(int i = 0; i < eigenvalues->size; i++) {
        printf("%.8f\n", gsl_vector_get(eigenvalues, i));
    }
    printf("Vectores propios: \n");
    for(int i = 0; i < eigenvectors->size1; i++) {
        for(int j = 0; j < eigenvectors->size2; j++) {
            printf("%.8f\t", gsl_matrix_get(eigenvectors, i, j));
        }
        printf("\n");
    }

    Matrix* datos_transformados = _create_Matrix(matrix->rows, matrix->cols);
    transform_data(matrix, eigenvectors, datos_transformados);

    gsl_vector_free(eigenvalues);
    gsl_matrix_free(eigenvectors);

    _copy_matrix(datos_transformados, matrix);
    _free_matrix(datos_transformados);
    _free_matrix(covariance);
}

int main(int argc, char *argv[]) {
    Matrix* matriz = _create_Matrix(4, 4);
    matriz->data[0][0] = 1.2; matriz->data[0][1] = 0.8; matriz->data[0][2] = 6.0; matriz->data[0][3] = 5.7;
    matriz->data[1][0] = 4.1; matriz->data[1][1] = 1.5; matriz->data[1][2] = 3.1; matriz->data[1][3] = 1.1;
    matriz->data[2][0] = 2.9; matriz->data[2][1] = 3.3; matriz->data[2][2] = 0.12; matriz->data[2][3] = 4.4;
    matriz->data[3][0] = 3.7; matriz->data[3][1] = 2.1; matriz->data[3][2] = 1.2; matriz->data[3][3] = 7.3;

    printf("Datos matriz inicial: \n");
    _print_matrix(matriz);

    do_pca(matriz);

    printf("Datos transformados (PCA aplicada): \n");
    _print_matrix(matriz);

    _free_matrix(matriz);
    return 0;
}
