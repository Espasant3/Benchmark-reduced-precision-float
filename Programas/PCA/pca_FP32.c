#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <cblas.h>
#include <lapacke.h>

#define N_SMALL 4

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
        float suma_desviaciones = 0;
        for(int i = 0; i < matrix->rows; i++) {
            suma += matrix->data[i][j];
        }
        medias[j] = suma / matrix->rows;
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

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            if (desviaciones[j] != 0) {
                matrix->data[i][j] = (matrix->data[i][j] - medias[j]) / desviaciones[j];
            } else {
                matrix->data[i][j] = 0;  // Evitar división por cero
            }
        }
    }

    // Asegurarse de liberar la memoria
    free(medias);
    free(desviaciones);
}

// Función para calcular la matriz de covarianza
void calculate_covariance(Matrix* matrix, Matrix* covariance) {
    for(int i = 0; i < matrix->cols; i++) {
        for(int j = 0; j < matrix->cols; j++) {
            float suma = 0.0f;
            for(int k = 0; k < matrix->rows; k++) {
                suma += (matrix->data[k][i] * matrix->data[k][j]);
            }
            covariance->data[i][j] = suma / (matrix->rows - 1);
        }
    }
}

// Función para ordenar los valores propios y vectores propios en orden descendente
void sort_eigenvalues_and_eigenvectors(int n, float* eigenvalues, float* eigenvectors) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (eigenvalues[j] < eigenvalues[j + 1]) {
                // Intercambiar valores propios
                float temp_value = eigenvalues[j];
                eigenvalues[j] = eigenvalues[j + 1];
                eigenvalues[j + 1] = temp_value;

                // Intercambiar columnas de vectores propios
                for (int k = 0; k < n; k++) {
                    float temp_vector = eigenvectors[k * n + j];
                    eigenvectors[k * n + j] = eigenvectors[k * n + j + 1];
                    eigenvectors[k * n + j + 1] = temp_vector;
                }
            }
        }
    }
}

// Función para calcular los valores y vectores propios de la matriz de covarianza
void calculate_eigenvalues_and_eigenvectors(Matrix* covariance, float *eigenvalues, float *eigenvectors) {
    int n = covariance->rows;
    int lda = n;

    // Copiar datos de la matriz de covarianza a eigenvectors (array plano)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            eigenvectors[i * n + j] = covariance->data[i][j];
        }
    }

    // Usar LAPACKE_ssyev para calcular valores y vectores propios
    int info = LAPACKE_ssyev(LAPACK_ROW_MAJOR, 'V', 'U', n, eigenvectors, lda, eigenvalues);

    if (info > 0) {
        printf("Error: LAPACKE_ssyev failed to converge. Info: %d\n", info);
        exit(EXIT_FAILURE);
    }
    
    // Ordenar valores propios y vectores propios
    sort_eigenvalues_and_eigenvectors(n, eigenvalues, eigenvectors);

}

// Función para transformar los datos usando los vectores propios
void transform_data(Matrix* matrix, float* eigenvectors, Matrix* transformed_data) {

    if (matrix->rows != transformed_data->rows || matrix->cols != transformed_data->cols) {
        printf("Error: Dimensiones incompatibles entre matrix y transformed_data.\n");
        _free_matrix(matrix);
        _free_matrix(transformed_data);
        free(eigenvectors);
        exit(EXIT_FAILURE);
    }

    // Asegurarse de que los punteros son válidos y alineados correctamente
    float* matrix_data = (float*)malloc(matrix->rows * matrix->cols * sizeof(float));
    float* transformed_data_data = (float*)malloc(transformed_data->rows * transformed_data->cols * sizeof(float));

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            matrix_data[i * matrix->cols + j] = matrix->data[i][j];
            transformed_data_data[i * transformed_data->cols + j] = transformed_data->data[i][j];
        }
    }

    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                matrix->rows, matrix->cols, matrix->cols, 
                1.0f, matrix_data, matrix->cols, 
                eigenvectors, matrix->cols, 
                0.0f, transformed_data_data, matrix->cols);

    // Copiar los datos de vuelta a `transformed_data`
    for (int i = 0; i < transformed_data->rows; i++) {
        for (int j = 0; j < transformed_data->cols; j++) {
            transformed_data->data[i][j] = transformed_data_data[i * transformed_data->cols + j];
        }
    }

    free(matrix_data);
    free(transformed_data_data);
}

// Función principal para realizar PCA
void do_pca(Matrix* matrix) {
    // Estandarizar la matriz
    standarize_matrix(matrix);

    // Crear matriz de covarianza
    Matrix* covariance = _create_Matrix(matrix->cols, matrix->cols);
    if (covariance == NULL) {
        printf("Error: No se pudo reservar memoria para la matriz de covarianza.\n");
        _free_matrix(matrix);
        exit(EXIT_FAILURE);
    }

    // Calcular la matriz de covarianza
    calculate_covariance(matrix, covariance);

    // Asignar memoria para eigenvalues y eigenvectors
    float* eigenvalues = (float*) malloc(covariance->rows * sizeof(float));
    float* eigenvectors = (float*) malloc(covariance->rows * covariance->cols * sizeof(float));

    // Calcular valores propios y vectores propios
    calculate_eigenvalues_and_eigenvectors(covariance, eigenvalues, eigenvectors);

    // Crear matriz para datos transformados
    Matrix* datos_transformados = _create_Matrix(matrix->rows, matrix->cols);

    // Transformar datos usando los vectores propios
    transform_data(matrix, eigenvectors, datos_transformados);

    // Liberar memoria de eigenvalues y eigenvectors
    free(eigenvalues);
    free(eigenvectors);

    // Copiar datos transformados de vuelta a la matriz original
    _copy_matrix(datos_transformados, matrix);

    // Liberar memoria
    _free_matrix(datos_transformados);
    _free_matrix(covariance);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Uso: %s <tamaño de la matriz nxn> [<seed>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);

    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);

    Matrix* matriz_small = _create_Matrix(N_SMALL, N_SMALL);

    if(matriz_small == NULL) {
        printf("Error: No se pudo reservar memoria para la matriz_small.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < matriz_small->rows; i++) {
        for (int j = 0; j < matriz_small->cols; j++) {
            matriz_small->data[i][j] = (float)rand() / RAND_MAX * 10.0f; // Genera números aleatorios entre 0 y 10
        }
    }
    
    printf("Datos matriz_small inicial: \n");
    _print_matrix(matriz_small);

    do_pca(matriz_small);

    printf("Datos transformados (PCA aplicada): \n");
    _print_matrix(matriz_small);

    _free_matrix(matriz_small);


    Matrix* matriz = _create_Matrix(n, n);

    if(matriz == NULL) {
        printf("Error: No se pudo reservar memoria para la matriz.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < matriz->rows; i++) {
        for (int j = 0; j < matriz->cols; j++) {
            matriz->data[i][j] = (float)rand() / RAND_MAX * 10.0f; // Genera números aleatorios entre 0 y 10
        }
    }

    //Para medir el tiempo de ejecución
    
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    
    /* 
        Código del programa cuyo tiempo quiero medir
    */
    do_pca(matriz);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);

    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f %.10e\n", matriz->data[n-1][n-1], matriz->data[n-1][n-1]);
    
    _free_matrix(matriz);

    return EXIT_SUCCESS;
}
