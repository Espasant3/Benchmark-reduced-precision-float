#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEGALL_53_WAVELET 1
#define CDF_97_WAVELET 2
#define N_SMALL 6

void convolve1d(float* input_vector, int vector_size, double* kernel, int kernel_size) {
    // Creamos un vector temporal para almacenar los resultados de la convolución
    float* temp_vector = (float*) malloc(vector_size * sizeof(float));

    // Inicializamos el vector temporal con ceros
    for (int i = 0; i < vector_size; i++) {
        temp_vector[i] = 0.0f;
    }

    // Calculamos la convolución
    for (int i = 0; i < vector_size; i++) {
        for (int j = 0; j < kernel_size; j++) {
            if (i + j < vector_size) {
                temp_vector[i] += input_vector[i + j] * (float)kernel[j];
            }
        }
    }

    // Copiamos los resultados de la convolución de vuelta a input_vector
    for (int i = 0; i < vector_size; i++) {
        input_vector[i] = temp_vector[i];
    }

    // Liberamos la memoria del vector temporal
    free(temp_vector);
}

void convolve1d_generic(float* input_vector, int vector_size, double* low_pass_kernel, int low_pass_size, double* high_pass_kernel, int high_pass_size) {
    // Creamos vectores temporales para almacenar los resultados de la convolución
    float* low_pass_result = (float*) malloc(vector_size * sizeof(float));
    float* high_pass_result = (float*) malloc(vector_size * sizeof(float));

    // Inicializamos los vectores temporales con ceros
    for (int i = 0; i < vector_size; i++) {
        low_pass_result[i] = 0.0f;
        high_pass_result[i] = 0.0f;
    }

    // Calculamos la convolución con el filtro pasa bajos
    for (int i = 0; i < vector_size; i++) {
        for (int j = 0; j < low_pass_size; j++) {
            if (i + j < vector_size) {
                low_pass_result[i] += input_vector[i + j] * (float)low_pass_kernel[j];
            }
        }
    }

    // Calculamos la convolución con el filtro pasa altos
    for (int i = 0; i < vector_size; i++) {
        for (int j = 0; j < high_pass_size; j++) {
            if (i + j < vector_size) {
                high_pass_result[i] += input_vector[i + j] * (float)high_pass_kernel[j];
            }
        }
    }


    // Submuestreamos y copiamos los resultados de la convolución de vuelta a input_vector
    for (int i = 0; i < vector_size / 2; i++) {
        input_vector[i] = low_pass_result[2 * i];
        input_vector[vector_size / 2 + i] = high_pass_result[2 * i];
    }

    // Liberamos la memoria de los vectores temporales
    free(low_pass_result);
    free(high_pass_result);
}
/**
 * @brief Initializes the kernels for the DWT.
 *
 * @param low_pass_kernel Pointer to the low-pass kernel array.
 * @param low_pass_size Pointer to the size of the low-pass kernel array.
 * @param high_pass_kernel Pointer to the high-pass kernel array.
 * @param high_pass_size Pointer to the size of the high-pass kernel array.
 * @param kernel_type The type of kernel to initialize (1: LeGall 5/3, 2: CDF 9/7).
 */


void initialize_kernels(double** low_pass_kernel, int* low_pass_size, double** high_pass_kernel, int* high_pass_size, int kernel_type) {
    switch (kernel_type) {
        case LEGALL_53_WAVELET: // LeGall 5/3 wavelet
            *low_pass_size = 5;
            *high_pass_size = 3;
            *low_pass_kernel = (double*) malloc(*low_pass_size * sizeof(double));
            *high_pass_kernel = (double*) malloc(*high_pass_size * sizeof(double));

            (*low_pass_kernel)[0] = -1.0/8;
            (*low_pass_kernel)[1] = 1.0/4;
            (*low_pass_kernel)[2] = 3.0/4;
            (*low_pass_kernel)[3] = 1.0/4;
            (*low_pass_kernel)[4] = -1.0/8;

            (*high_pass_kernel)[0] = -1.0/2;
            (*high_pass_kernel)[1] = 1.0;
            (*high_pass_kernel)[2] = -1.0/2;
            break;

        case CDF_97_WAVELET: // CDF 9/7 wavelet (lossy)
            *low_pass_size = 9;
            *high_pass_size = 7;
            *low_pass_kernel = (double*) malloc(*low_pass_size * sizeof(double));
            *high_pass_kernel = (double*) malloc(*high_pass_size * sizeof(double));

            (*low_pass_kernel)[0] = 0.026748757411;
            (*low_pass_kernel)[1] = -0.016864118443;
            (*low_pass_kernel)[2] = -0.078223266529;
            (*low_pass_kernel)[3] = 0.266864118443;
            (*low_pass_kernel)[4] = 0.602949018236;
            (*low_pass_kernel)[5] = 0.266864118443;
            (*low_pass_kernel)[6] = -0.078223266529;
            (*low_pass_kernel)[7] = -0.016864118443;
            (*low_pass_kernel)[8] = 0.026748757411;

            (*high_pass_kernel)[0] = 0.091271763114;
            (*high_pass_kernel)[1] = -0.057543526229;
            (*high_pass_kernel)[2] = -0.591271763114;
            (*high_pass_kernel)[3] = 1.11508705;
            (*high_pass_kernel)[4] = -0.591271763114;
            (*high_pass_kernel)[5] = -0.057543526229;
            (*high_pass_kernel)[6] = 0.091271763114;
            break;

        default:
            printf("Invalid kernel type\n");
            exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Uso: %s <tamaño del vector> [<seed>]\n", argv[0]);
        return EXIT_FAILURE;
    }  

    int vector_size = atoi(argv[1]);

    // Vector de entrada de ejemplo
    float* input_vector_small = (float*) malloc(N_SMALL * sizeof(float));
    float* aux_vector_small = (float*) malloc(N_SMALL * sizeof(float));

    // Se usa una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);

    for (int i = 0; i < N_SMALL; i++) {
        aux_vector_small[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
    }

    // Copiamos los datos del vector auxiliar al vector de entrada
    for (int i = 0; i < N_SMALL; i++) {
        input_vector_small[i] = aux_vector_small[i];
    }

    printf("Array input_vector_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", input_vector_small[i]);
    }
    printf("]\n");

    // Variables para los kernels y sus tamaños
    double* low_pass_kernel;
    double* high_pass_kernel;
    int low_pass_size;
    int high_pass_size;

    // Inicializamos los kernels para LeGall 5/3 wavelet
    initialize_kernels(&low_pass_kernel, &low_pass_size, &high_pass_kernel, &high_pass_size, LEGALL_53_WAVELET);

    // Ejemplo de uso de la función con los filtros LeGall 5/3
    printf("Convolving with LeGall 5/3 Wavelet\n");
    convolve1d_generic(input_vector_small, N_SMALL, low_pass_kernel, low_pass_size, high_pass_kernel, high_pass_size);

    // Imprimimos el resultado
    printf("Result: ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", input_vector_small[i]);
    }
    printf("\n");

    // Liberamos la memoria de los kernels
    free(low_pass_kernel);
    free(high_pass_kernel);

    // Restablecemos el vector de entrada para el siguiente ejemplo
    for (int i = 0; i < N_SMALL; i++) {
        input_vector_small[i] = aux_vector_small[i];
    }

    // Inicializamos los kernels para CDF 9/7 wavelet (lossy)
    initialize_kernels(&low_pass_kernel, &low_pass_size, &high_pass_kernel, &high_pass_size, CDF_97_WAVELET);

    // Ejemplo de uso de la función con los filtros CDF 9/7 (lossy)
    printf("Convolving with CDF 9/7 Wavelet (lossy)\n");
    convolve1d_generic(input_vector_small, N_SMALL, low_pass_kernel, low_pass_size, high_pass_kernel, high_pass_size);

    // Imprimimos el resultado
    printf("Result: ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", input_vector_small[i]);
    }
    printf("\n");

    // Liberamos la memoria del vector de entrada y los kernels
    free(input_vector_small);
    free(aux_vector_small);
    free(low_pass_kernel);
    free(high_pass_kernel);


    
    // Vector de entrada de tamaño grande
    float* input_vector = (float*) malloc(vector_size * sizeof(float));
    float* aux_vector = (float*) malloc(vector_size * sizeof(float));

    // Generamos nuevos datos para el vector grande
    for (int i = 0; i < vector_size; i++) {
        aux_vector[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
    }

    // Copiamos los datos del vector auxiliar al vector de entrada
    for (int i = 0; i < vector_size; i++) {
        input_vector[i] = aux_vector[i];
    }

    printf("Array input_vector: [ ");
    for (int i = 0; i < vector_size; i++) {
        printf("%f ", input_vector[i]);
    }
    printf("]\n");

    // Inicializamos los kernels para LeGall 5/3 wavelet
    initialize_kernels(&low_pass_kernel, &low_pass_size, &high_pass_kernel, &high_pass_size, LEGALL_53_WAVELET);

    // Ejemplo de uso de la función con los filtros LeGall 5/3
    printf("Convolving large vector with LeGall 5/3 Wavelet\n");
    convolve1d_generic(input_vector, vector_size, low_pass_kernel, low_pass_size, high_pass_kernel, high_pass_size);

    // Imprimimos el resultado
    printf("Result: ");
    for (int i = 0; i < vector_size; i++) {
        printf("%f ", input_vector[i]);
    }
    printf("\n");

    // Liberamos la memoria de los kernels
    free(low_pass_kernel);
    free(high_pass_kernel);

    // Restablecemos el vector de entrada para el siguiente ejemplo
    for (int i = 0; i < vector_size; i++) {
        input_vector[i] = aux_vector[i];
    }

    // Inicializamos los kernels para CDF 9/7 wavelet (lossy)
    initialize_kernels(&low_pass_kernel, &low_pass_size, &high_pass_kernel, &high_pass_size, CDF_97_WAVELET);

    // Ejemplo de uso de la función con los filtros CDF 9/7 (lossy)
    printf("Convolving large vector with CDF 9/7 Wavelet (lossy)\n");
    convolve1d_generic(input_vector, vector_size, low_pass_kernel, low_pass_size, high_pass_kernel, high_pass_size);

    // Imprimimos el resultado
    printf("Result: ");
    for (int i = 0; i < vector_size; i++) {
        printf("%f ", input_vector[i]);
    }
    printf("\n");

    // Liberamos la memoria del vector de entrada y los kernels
    free(input_vector);
    free(aux_vector);
    free(low_pass_kernel);
    free(high_pass_kernel);

    return EXIT_SUCCESS;
}
