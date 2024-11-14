#include <stdio.h>
#include <stdlib.h>

void convolve1d(double* input_vector, int vector_size, double* kernel, int kernel_size) {
    // Creamos un vector temporal para almacenar los resultados de la convolución
    double* temp_vector = (double*) malloc(vector_size * sizeof(double));

    // Inicializamos el vector temporal con ceros
    for (int i = 0; i < vector_size; i++) {
        temp_vector[i] = 0.0;
    }

    // Calculamos la convolución
    for (int i = 0; i < vector_size; i++) {
        for (int j = 0; j < kernel_size; j++) {
            if (i + j < vector_size) {
                temp_vector[i] += input_vector[i + j] * kernel[j];
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

void convolve1d_generic(double* input_vector, int vector_size, double* low_pass_kernel, int low_pass_size, double* high_pass_kernel, int high_pass_size) {
    // Creamos vectores temporales para almacenar los resultados de la convolución
    double* low_pass_result = (double*) malloc(vector_size * sizeof(double));
    double* high_pass_result = (double*) malloc(vector_size * sizeof(double));

    // Inicializamos los vectores temporales con ceros
    for (int i = 0; i < vector_size; i++) {
        low_pass_result[i] = 0.0;
        high_pass_result[i] = 0.0;
    }

    // Calculamos la convolución con el filtro pasa bajos
    for (int i = 0; i < vector_size; i++) {
        for (int j = 0; j < low_pass_size; j++) {
            if (i + j < vector_size) {
                low_pass_result[i] += input_vector[i + j] * low_pass_kernel[j];
            }
        }
    }

    // Calculamos la convolución con el filtro pasa altos
    for (int i = 0; i < vector_size; i++) {
        for (int j = 0; j < high_pass_size; j++) {
            if (i + j < vector_size) {
                high_pass_result[i] += input_vector[i + j] * high_pass_kernel[j];
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

int main() {
    // Vector de entrada de ejemplo
    double input_vector[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    int vector_size = sizeof(input_vector) / sizeof(input_vector[0]);

    // Coeficientes para LeGall 5/3 wavelet
    double low_pass_legall[] = {-1.0/8, 1.0/4, 3.0/4, 1.0/4, -1.0/8};
    double high_pass_legall[] = {-1.0/2, 1.0, -1.0/2};

    // Coeficientes para CDF 9/7 wavelet (lossy)
    double low_pass_cdf97[] = {0.026748757411, -0.016864118443, -0.078223266529, 0.266864118443, 0.602949018236, 0.266864118443, -0.078223266529, -0.016864118443, 0.026748757411};
    double high_pass_cdf97[] = {0.091271763114, -0.057543526229, -0.591271763114, 1.11508705, -0.591271763114, -0.057543526229, 0.091271763114};

    // Coeficientes para la transformada inversa de CDF 9/7 wavelet (lossy)
    double low_pass_cdf97_inv[] = {0.091271763114, 0.057543526229, -0.591271763114, -1.11508705, -0.591271763114, 0.057543526229, 0.091271763114};
    double high_pass_cdf97_inv[] = {0.026748757411, 0.016864118443, -0.078223266529, -0.266864118443, 0.602949018236, -0.266864118443, -0.078223266529, 0.016864118443, 0.026748757411};

    // Tamaños de los kernels
    int low_pass_legall_size = sizeof(low_pass_legall) / sizeof(low_pass_legall[0]);
    int high_pass_legall_size = sizeof(high_pass_legall) / sizeof(high_pass_legall[0]);
    int low_pass_cdf97_size = sizeof(low_pass_cdf97) / sizeof(low_pass_cdf97[0]);
    int high_pass_cdf97_size = sizeof(high_pass_cdf97) / sizeof(high_pass_cdf97[0]);
    int low_pass_cdf97_inv_size = sizeof(low_pass_cdf97_inv) / sizeof(low_pass_cdf97_inv[0]);
    int high_pass_cdf97_inv_size = sizeof(high_pass_cdf97_inv) / sizeof(high_pass_cdf97_inv[0]);

    // Ejemplo de uso de la función con los filtros LeGall 5/3
    printf("Convolving with LeGall 5/3 Wavelet\n");
    convolve1d_generic(input_vector, vector_size, low_pass_legall, low_pass_legall_size, high_pass_legall, high_pass_legall_size);

    // Restablecemos el vector de entrada para el siguiente ejemplo
    double input_vector_reset[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    for (int i = 0; i < vector_size; i++) {
        input_vector[i] = input_vector_reset[i];
    }

    // Ejemplo de uso de la función con los filtros CDF 9/7 (lossy)
    printf("Convolving with CDF 9/7 Wavelet (lossy)\n");
    convolve1d_generic(input_vector, vector_size, low_pass_cdf97, low_pass_cdf97_size, high_pass_cdf97, high_pass_cdf97_size);

    return 0;
}
