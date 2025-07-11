#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <lapacke.h>
#include <arm_fp16.h>
#include <armpl.h> // Esta es para usar cblas_hgemm

#define N_SMALL 4

// Estructura para representar una matriz
typedef struct {
    int rows;
    int cols;
    __fp16** data;
} Matrix;

// Función para crear una estructura Matrix de tamaño rows x cols
Matrix* _create_Matrix(int rows, int cols) {
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = malloc(rows * sizeof(__fp16 *));
    for(int i = 0; i < rows; i++) {
        matrix->data[i] = (__fp16 *)calloc(cols, sizeof(__fp16));
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
void _calc_means_and_deviations(Matrix* matrix, __fp16 *medias, __fp16 *desviaciones) {
    for(int j = 0; j < matrix->cols; j++) {
        __fp16 suma = 0;
        __fp16 suma_desviaciones = 0;
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
    __fp16 *medias = (__fp16 *)calloc(matrix->cols, sizeof(__fp16));
    __fp16 *desviaciones = (__fp16 *)calloc(matrix->cols, sizeof(__fp16));
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
void swap(__fp16* a, __fp16* b) {
    __fp16 temp = *a;
    *a = *b;
    *b = temp;
}

// Función auxiliar para intercambiar dos columnas en una matriz
void swap_columns(__fp16* eigenvectors, int n, int col1, int col2) {
    for (int i = 0; i < n; i++) {
        __fp16 temp = eigenvectors[i * n + col1];
        eigenvectors[i * n + col1] = eigenvectors[i * n + col2];
        eigenvectors[i * n + col2] = temp;
    }
}

// Función auxiliar para particionar el array de valores propios
int partition(__fp16* eigenvalues, __fp16* eigenvectors, int n, int low, int high) {
    __fp16 pivot = eigenvalues[high];
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
void quicksort(__fp16* eigenvalues, __fp16* eigenvectors, int n, int low, int high) {
    if (low < high) {
        int pi = partition(eigenvalues, eigenvectors, n, low, high);
        quicksort(eigenvalues, eigenvectors, n, low, pi - 1);
        quicksort(eigenvalues, eigenvectors, n, pi + 1, high);
    }
}

// Función para ordenar los valores propios y vectores propios en orden descendente
void sort_eigenvalues_and_eigenvectors(int n, __fp16* eigenvalues, __fp16* eigenvectors) {
    quicksort(eigenvalues, eigenvectors, n, 0, n - 1);
}

// Función para calcular los valores y vectores propios de la matriz de covarianza
void calculate_eigenvalues_and_eigenvectors(Matrix* covariance, __fp16 *eigenvalues, __fp16 *eigenvectors) {
    
    int n = covariance->rows;
    int lda = n;
    int covariance_size = covariance->rows * covariance->cols;

    float* eigenvectors_f = (float*) calloc(covariance_size, sizeof(float));
    float* eigenvalues_f = (float*) calloc(n, sizeof(float));

    if (eigenvectors_f == NULL || eigenvalues_f == NULL) {
        printf("Error: No se pudo reservar memoria para eigenvalues o eigenvectors (float).\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < covariance_size; i++) {
        eigenvectors_f[i] = (float)covariance->data[i / covariance->cols][i % covariance->cols];
    }

    // Usar LAPACKE_ssyev para calcular valores y vectores propios
    int info = LAPACKE_ssyev(LAPACK_ROW_MAJOR, 'V', 'U', n, eigenvectors_f, lda, eigenvalues_f);

    if (info > 0) {
        printf("Error: LAPACKE_ssyev failed to converge. Info: %d\n", info);
        exit(EXIT_FAILURE);
    }

    // Copiar valores de eigenvalues_f a eigenvalues
    for (int i = 0; i < n; i++) {
        eigenvalues[i] = (__fp16)eigenvalues_f[i];
    }

    // Copiar valores de eigenvectors_f a eigenvectors (array plano)
    for (int i = 0; i < covariance_size; i++) {
        eigenvectors[i] = (__fp16)eigenvectors_f[i];
    }

    free(eigenvectors_f);
    free(eigenvalues_f);

    // Ordenar valores propios y vectores propios
    sort_eigenvalues_and_eigenvectors(covariance->rows, eigenvalues, eigenvectors);
}

// Función para transformar los datos usando los vectores propios
void transform_data(Matrix* matrix, __fp16* eigenvectors, Matrix* transformed_data) {

    if (matrix->rows != transformed_data->rows || matrix->cols != transformed_data->cols) {
        printf("Error: Dimensiones incompatibles entre matrix y transformed_data.\n");
        _free_matrix(matrix);
        _free_matrix(transformed_data);
        free(eigenvectors);
        exit(EXIT_FAILURE);
    }

    int matrix_size = matrix->rows * matrix->cols;
    int transformed_size = transformed_data->rows * transformed_data->cols;

    // Asegurarse de que los punteros son válidos y alineados correctamente
    __fp16* matrix_data = (__fp16*)calloc(matrix_size, sizeof(__fp16));
    __fp16* transformed_data_data = (__fp16*)calloc(transformed_size, sizeof(__fp16));

    // Inicializar los arrays temporales
    for (int i = 0; i < matrix_size; i++) {
        matrix_data[i] = matrix->data[i / matrix->cols][i % matrix->cols];
    }

    cblas_hgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                matrix->rows, matrix->cols, matrix->cols, 
                1.0f, matrix_data, matrix->cols, 
                eigenvectors, matrix->cols, 
                0.0f, transformed_data_data, matrix->cols);

    // Copiar los datos de vuelta a transformed_data
    for (int i = 0; i < transformed_size; i++) {
        transformed_data->data[i / transformed_data->cols][i % transformed_data->cols] = transformed_data_data[i];
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
    __fp16* eigenvalues = (__fp16*) calloc(covariance->rows, sizeof(__fp16));
    __fp16* eigenvectors = (__fp16*) calloc(covariance->rows * covariance->cols, sizeof(__fp16));


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
            matriz_small->data[i][j] = (__fp16)temp;
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
            matriz->data[i][j] = (__fp16)temp;
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
