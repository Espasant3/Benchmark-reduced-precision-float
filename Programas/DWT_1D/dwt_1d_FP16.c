#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LEGALL_53_WAVELET 1
#define CDF_97_WAVELET 2
#define N_SMALL 6

typedef struct {
    double* low_pass_kernel;
    int low_pass_size;
    double* high_pass_kernel;
    int high_pass_size;
} WaveletKernels;

void convolve1d_generic(_Float16* input_vector, int vector_size, WaveletKernels kernels) {
    _Float16* low_pass_result = (_Float16*) malloc(vector_size * sizeof(_Float16));
    _Float16* high_pass_result = (_Float16*) malloc(vector_size * sizeof(_Float16));

    for (int i = 0; i < vector_size; i++) {
        low_pass_result[i] = 0.0f;
        high_pass_result[i] = 0.0f;
    }

    for (int i = 0; i < vector_size; i++) {
        for (int j = 0; j < kernels.low_pass_size; j++) {
            if (i + j < vector_size) {
                low_pass_result[i] += input_vector[i + j] * (_Float16)kernels.low_pass_kernel[j];
            }
        }
    }

    for (int i = 0; i < vector_size; i++) {
        for (int j = 0; j < kernels.high_pass_size; j++) {
            if (i + j < vector_size) {
                high_pass_result[i] += input_vector[i + j] * (_Float16)kernels.high_pass_kernel[j];
            }
        }
    }

    for (int i = 0; i < vector_size / 2; i++) {
        input_vector[i] = low_pass_result[2 * i];
        input_vector[vector_size / 2 + i] = high_pass_result[2 * i];
    }

    free(low_pass_result);
    free(high_pass_result);
}

void initialize_kernels(WaveletKernels* kernels, int kernel_type) {
    switch (kernel_type) {
        case LEGALL_53_WAVELET:
            kernels->low_pass_size = 5;
            kernels->high_pass_size = 3;
            kernels->low_pass_kernel = (double*) malloc(kernels->low_pass_size * sizeof(double));
            kernels->high_pass_kernel = (double*) malloc(kernels->high_pass_size * sizeof(double));

            kernels->low_pass_kernel[0] = -1.0/8;
            kernels->low_pass_kernel[1] = 1.0/4;
            kernels->low_pass_kernel[2] = 3.0/4;
            kernels->low_pass_kernel[3] = 1.0/4;
            kernels->low_pass_kernel[4] = -1.0/8;

            kernels->high_pass_kernel[0] = -1.0/2;
            kernels->high_pass_kernel[1] = 1.0;
            kernels->high_pass_kernel[2] = -1.0/2;
            break;

        case CDF_97_WAVELET:
            kernels->low_pass_size = 9;
            kernels->high_pass_size = 7;
            kernels->low_pass_kernel = (double*) malloc(kernels->low_pass_size * sizeof(double));
            kernels->high_pass_kernel = (double*) malloc(kernels->high_pass_size * sizeof(double));

            kernels->low_pass_kernel[0] = 0.026748757411;
            kernels->low_pass_kernel[1] = -0.016864118443;
            kernels->low_pass_kernel[2] = -0.078223266529;
            kernels->low_pass_kernel[3] = 0.266864118443;
            kernels->low_pass_kernel[4] = 0.602949018236;
            kernels->low_pass_kernel[5] = 0.266864118443;
            kernels->low_pass_kernel[6] = -0.078223266529;
            kernels->low_pass_kernel[7] = -0.016864118443;
            kernels->low_pass_kernel[8] = 0.026748757411;

            kernels->high_pass_kernel[0] = 0.091271763114;
            kernels->high_pass_kernel[1] = -0.057543526229;
            kernels->high_pass_kernel[2] = -0.591271763114;
            kernels->high_pass_kernel[3] = 1.11508705;
            kernels->high_pass_kernel[4] = -0.591271763114;
            kernels->high_pass_kernel[5] = -0.057543526229;
            kernels->high_pass_kernel[6] = 0.091271763114;
            break;

        default:
            printf("Invalid kernel type\n");
            exit(EXIT_FAILURE);
    }
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

    _Float16* input_vector_small = (_Float16*) malloc(N_SMALL * sizeof(_Float16));
    _Float16* aux_vector_small = (_Float16*) malloc(N_SMALL * sizeof(_Float16));

    unsigned int seed = (optind + 1 < argc) ? (unsigned int)atoi(argv[optind + 1]) : (unsigned int)time(NULL);
    srand(seed);

    for (int i = 0; i < N_SMALL; i++) {
        float temp_value = ((float)rand() / (float)(RAND_MAX)) * 10.0;
        aux_vector_small[i] = (_Float16)temp_value;
    }

    for (int i = 0; i < N_SMALL; i++) {
        input_vector_small[i] = aux_vector_small[i];
    }

    printf("Array input_vector_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)input_vector_small[i]);
    }
    printf("]\n");

    WaveletKernels kernels;

    initialize_kernels(&kernels, LEGALL_53_WAVELET);

    printf("Convolving with LeGall 5/3 Wavelet\n");
    convolve1d_generic(input_vector_small, N_SMALL, kernels);

    printf("Result: ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)input_vector_small[i]);
    }
    printf("\n");

    free(kernels.low_pass_kernel);
    free(kernels.high_pass_kernel);

    for (int i = 0; i < N_SMALL; i++) {
        input_vector_small[i] = aux_vector_small[i];
    }

    initialize_kernels(&kernels, CDF_97_WAVELET);

    printf("Convolving with CDF 9/7 Wavelet (lossy)\n");
    convolve1d_generic(input_vector_small, N_SMALL, kernels);

    printf("Result: ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)input_vector_small[i]);
    }
    printf("\n");

    free(input_vector_small);
    free(aux_vector_small);
    free(kernels.low_pass_kernel);
    free(kernels.high_pass_kernel);

    // Fin del programa para un vector pequeño

    _Float16* input_vector = (_Float16*) malloc(n * sizeof(_Float16));
    _Float16* aux_vector = (_Float16*) malloc(n * sizeof(_Float16));

    for (int i = 0; i < n; i++) {
        float temp_value = ((float)rand() / (float)(RAND_MAX)) * 10.0;
        aux_vector[i] = (_Float16)temp_value;
    }

    for (int i = 0; i < n; i++) {
        input_vector[i] = aux_vector[i];
    }

    initialize_kernels(&kernels, LEGALL_53_WAVELET);

    printf("Convolving large vector with LeGall 5/3 Wavelet\n");

    if(verbose){
        printf("Datos ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", (float)input_vector[i]);
        }
        printf("\n");
    }

    //Para medir el tiempo de ejecución

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    /* 
        Código del programa cuyo tiempo quiero medir
    */

    convolve1d_generic(input_vector, n, kernels);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);


    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f %.10e\n", (float)input_vector[n-1], (float)input_vector[n-1]);

    if(verbose){
        printf("Resultados ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", (float)input_vector[i]);
        }
        printf("\n");
    }

    free(kernels.low_pass_kernel);
    free(kernels.high_pass_kernel);

    for (int i = 0; i < n; i++) {
        input_vector[i] = aux_vector[i];
    }

    initialize_kernels(&kernels, CDF_97_WAVELET);

    printf("Convolving large vector with CDF 9/7 Wavelet (lossy)\n");

    if(verbose){
        printf("Datos ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", (float)input_vector[i]);
        }
        printf("\n");
    }

    //Para medir el tiempo de ejecución

    start = clock();

    /* 
        Código del programa cuyo tiempo quiero medir
    */

    convolve1d_generic(input_vector, n, kernels);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);

    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f %.10e\n", (float)input_vector[n-1], (float)input_vector[n-1]);

    if(verbose){
        printf("Resultados ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", (float)input_vector[i]);
        }
        printf("\n");
    }

    free(input_vector);
    free(aux_vector);
    free(kernels.low_pass_kernel);
    free(kernels.high_pass_kernel);

    return EXIT_SUCCESS;
}
