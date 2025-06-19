#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#ifdef __aarch64__
#include <arm_fp16.h>
#include <arm_bf16.h>
#include <armpl.h> // Esta es para usar cblas_hgemm
#else
#include <cblas.h>
#endif

#include "./functions-adapted/include/pca_reimpl.h"

#define N_SMALL 4

// Estructura para representar una matriz
typedef struct {
    int rows;
    int cols;
    __bf16** data;
} Matrix;

// Función para crear una estructura Matrix de tamaño rows x cols
Matrix* _create_Matrix(int rows, int cols) {
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = malloc(rows * sizeof(__bf16 *));
    for(int i = 0; i < rows; i++) {
        matrix->data[i] = (__bf16 *)calloc(cols, sizeof(__bf16));
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
            printf("%f\t", (float)matrix->data[i][j]);
        }
        printf("\n");
    }
}

// Función para imprimir una matriz con más decimales
void _print_matrix_exp(Matrix* matrix) {
    for(int i = 0; i < matrix->rows; i++) {
        printf("\t");
        for(int j = 0; j < matrix->cols; j++) {
            printf("%.10e  ", (float)matrix->data[i][j]);
        }
        printf("\n");
    }
}

// Función para calcular las medias y desviaciones estándar de cada columna de la matriz
void _calc_means_and_deviations(Matrix* matrix, __bf16 *medias, __bf16 *desviaciones) {
    for(int j = 0; j < matrix->cols; j++) {
        __bf16 suma = 0;
        __bf16 suma_desviaciones = 0;
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

// Función para copiar una matriz a otra
void _copy_matrix(Matrix* source, Matrix* destination) {
    for(int i = 0; i < source->rows; i++) {
        for(int j = 0; j < source->cols; j++) {
            destination->data[i][j] = source->data[i][j];
        }
    }
}

// Función para estandarizar la matriz
void standarize_matrix(Matrix* matrix) {
    __bf16 *medias = (__bf16 *)calloc(matrix->cols, sizeof(__bf16));
    __bf16 *desviaciones = (__bf16 *)calloc(matrix->cols, sizeof(__bf16));
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

// Función auxiliar para intercambiar dos elementos en un array
void swap(__bf16* a, __bf16* b) {
    __bf16 temp = *a;
    *a = *b;
    *b = temp;
}

// Función auxiliar para intercambiar dos columnas en una matriz
void swap_columns(__bf16* eigenvectors, int n, int col1, int col2) {
    for (int i = 0; i < n; i++) {
        __bf16 temp = eigenvectors[i * n + col1];
        eigenvectors[i * n + col1] = eigenvectors[i * n + col2];
        eigenvectors[i * n + col2] = temp;
    }
}

// Función auxiliar para particionar el array de valores propios
int partition(__bf16* eigenvalues, __bf16* eigenvectors, int n, int low, int high) {
    __bf16 pivot = eigenvalues[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (eigenvalues[j] > pivot) {
            i++;
            swap(&eigenvalues[i], &eigenvalues[j]);
            swap_columns(eigenvectors, n, i, j);
        }
    }
    swap(&eigenvalues[i + 1], &eigenvalues[high]);
    swap_columns(eigenvectors, n, i + 1, high);
    return i + 1;
}

// Función auxiliar para aplicar quicksort
void quicksort(__bf16* eigenvalues, __bf16* eigenvectors, int n, int low, int high) {
    if (low < high) {
        int pi = partition(eigenvalues, eigenvectors, n, low, high);
        quicksort(eigenvalues, eigenvectors, n, low, pi - 1);
        quicksort(eigenvalues, eigenvectors, n, pi + 1, high);
    }
}

// Función para ordenar los valores propios y vectores propios en orden descendente
void sort_eigenvalues_and_eigenvectors(int n, __bf16* eigenvalues, __bf16* eigenvectors) {
    quicksort(eigenvalues, eigenvectors, n, 0, n - 1);
}

// Función para calcular los valores y vectores propios de la matriz de covarianza
void calculate_eigenvalues_and_eigenvectors(Matrix* covariance, __bf16 *eigenvalues, __bf16 *eigenvectors) {
    
    int n = covariance->rows;
    int lda = n;

    // Usar LAPACKE_hfsyev para calcular valores y vectores propios
    int info = LAPACKE_hfsyev(LAPACK_ROW_MAJOR, 'V', 'U', n, eigenvectors, lda, eigenvalues);

    if (info > 0) {
        printf("Error: LAPACKE_hfsyev failed to converge. Info: %d\n", info);
        exit(EXIT_FAILURE);
    }

    // Ordenar valores propios y vectores propios
    sort_eigenvalues_and_eigenvectors(covariance->rows, eigenvalues, eigenvectors);
}

// Función para transformar los datos usando los vectores propios
void transform_data(Matrix* matrix, __bf16* eigenvectors, Matrix* transformed_data) {

    if (matrix->rows != transformed_data->rows || matrix->cols != transformed_data->cols) {
        printf("Error: Dimensiones incompatibles entre matrix y transformed_data.\n");
        _free_matrix(matrix);
        _free_matrix(transformed_data);
        free(eigenvectors);
        exit(EXIT_FAILURE);
    }

    int matrix_size = matrix->rows * matrix->cols;
    int transformed_size = transformed_data->rows * transformed_data->cols;


    #ifdef __aarch64__

    // Asegurarse de que los punteros son válidos y alineados correctamente
    __fp16* matrix_data = (__fp16*)calloc(matrix_size, sizeof(__fp16));
    __fp16* transformed_data_data = (__fp16*)calloc(transformed_size, sizeof(__fp16));
    __fp16* eigenvectors_f = (__fp16*)calloc(matrix->cols * matrix->cols, sizeof(__fp16));

    if (matrix_data == NULL || transformed_data_data == NULL) {
        printf("Error: No se pudo reservar memoria para matrix_data o transformed_data_data.\n");
        free(matrix_data);
        free(transformed_data_data);
        exit(EXIT_FAILURE);
    }

    // Inicializar los arrays temporales
    for (int i = 0; i < matrix_size; i++) {
        matrix_data[i] = (__fp16)matrix->data[i / matrix->cols][i % matrix->cols];
    }

    for(int i = 0; i < matrix->cols * matrix->cols; i++) {
        eigenvectors_f[i] = (__fp16)eigenvectors[i];
    }


    cblas_hgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                matrix->rows, matrix->cols, matrix->cols, 
                1.0f, matrix_data, matrix->cols, 
                eigenvectors_f, matrix->cols, 
                0.0f, transformed_data_data, matrix->cols);

    // Copiar los datos de vuelta a transformed_data
    for (int i = 0; i < transformed_size; i++) {
        transformed_data->data[i / transformed_data->cols][i % transformed_data->cols] = (__bf16)transformed_data_data[i];
    }

    for(int i = 0; i < matrix->cols * matrix->cols; i++) {
        eigenvectors[i] = (__bf16)eigenvectors_f[i];
    }

    free(matrix_data);
    free(transformed_data_data);
    free(eigenvectors_f);

    #else

    // Sección para x86_64
    float* matrix_data = (float*)calloc(matrix_size, sizeof(float));
    float* transformed_data_data = (float*)calloc(transformed_size, sizeof(float));
    float* eigenvectors_f = (float*)calloc(matrix->cols * matrix->cols, sizeof(float));

    if (matrix_data == NULL || transformed_data_data == NULL) {
        printf("Error: No se pudo reservar memoria para matrix_data o transformed_data_data.\n");
        free(matrix_data);
        free(transformed_data_data);
        exit(EXIT_FAILURE);
    }

    // Inicializar los arrays temporales
    for (int i = 0; i < matrix_size; i++) {
        matrix_data[i] = (float)matrix->data[i / matrix->cols][i % matrix->cols];
    }

    for(int i = 0; i < matrix->cols * matrix->cols; i++) {
        eigenvectors_f[i] = (float)eigenvectors[i];
    }

    // Verificar que las dimensiones son compatibles con cblas_sgemm
    if (matrix->rows <= 0 || transformed_data->cols <= 0 || matrix->cols <= 0) {
        printf("Error: Dimensiones no válidas para la multiplicación de matrices.\n");
        free(matrix_data);
        free(transformed_data_data);
        exit(EXIT_FAILURE);
    }

    // Realizar la multiplicación de matrices utilizando BLAS
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                matrix->rows, transformed_data->cols, matrix->cols,
                1.0f, matrix_data, matrix->cols,
                eigenvectors_f, matrix->cols,
                0.0f, transformed_data_data, transformed_data->cols);

    // Copiar los datos de vuelta a transformed_data
    for (int i = 0; i < transformed_size; i++) {
        transformed_data->data[i / transformed_data->cols][i % transformed_data->cols] = (__bf16)transformed_data_data[i];
    }
    
    for(int i = 0; i < matrix->cols * matrix->cols; i++) {
        eigenvectors[i] = (__bf16)eigenvectors_f[i];
    }

    free(matrix_data);
    free(transformed_data_data);
    free(eigenvectors_f);


    #endif
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
    __bf16* eigenvalues = (__bf16*) calloc(covariance->rows, sizeof(__bf16));
    __bf16* eigenvectors = (__bf16*) calloc(covariance->rows * covariance->cols, sizeof(__bf16));


    if (eigenvalues == NULL || eigenvectors == NULL) {
        printf("Error: No se pudo reservar memoria para eigenvalues y eigenvectors.\n");
        _free_matrix(matrix);
        _free_matrix(covariance);
        exit(EXIT_FAILURE);
    }

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

    int verbose = 0;
    int opt;

    // Manejar opciones (-v)
    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            default:
                fprintf(stderr, "Uso: %s [-v] <tamaño del vector> [<seed>]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Verificar argumentos restantes (tamaño y seed)
    if (optind >= argc) {
        fprintf(stderr, "Uso: %s [-v] <tamaño del vector> [<seed>]\n", argv[0]);
        return EXIT_FAILURE;
    }   

    int n = -1;

    n = atoi(argv[optind]);

    if (n <= 0) {
        fprintf(stderr, "El tamaño del vector debe ser un número entero positivo.\n");
        return EXIT_FAILURE;
    }

    unsigned int seed = (optind + 1 < argc) ? (unsigned int)atoi(argv[optind + 1]) : (unsigned int)time(NULL);
    srand(seed);

    Matrix* matriz_small = _create_Matrix(N_SMALL, N_SMALL);

    if(matriz_small == NULL) {
        printf("Error: No se pudo reservar memoria para la matriz_small.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < matriz_small->rows; i++) {
        for (int j = 0; j < matriz_small->cols; j++) {
            float temp = (float)rand() / RAND_MAX * 10.0f; // Genera números aleatorios entre 0 y 10
            matriz_small->data[i][j] = (__bf16)temp;
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
            float temp = (float)rand() / RAND_MAX * 10.0f; // Genera números aleatorios entre 0 y 10
            matriz->data[i][j] = (__bf16)temp;
        }
    }

    if(verbose){
        printf("Datos ejecucion: \n");
        _print_matrix_exp(matriz);
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

    printf("%f %.10e\n", (float)matriz->data[n-1][n-1], (float)matriz->data[n-1][n-1]);
    
    if(verbose){
        printf("Resultados ejecucion: \n");
        _print_matrix_exp(matriz);
    }

    _free_matrix(matriz);

    return EXIT_SUCCESS;
}
