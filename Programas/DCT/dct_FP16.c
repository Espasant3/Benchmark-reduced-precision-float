#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


// Definir el tamaño de la señal
#define N_SMALL 5

void dct(_Float16 *input, _Float16 *output, int n_size) {
    _Float16 alpha;
    _Float16 sum;
    for (int k = 0; k < n_size; k++) {
        if (k == 0) {
            alpha = sqrtf(1.0 / n_size);
        } else {
            alpha = sqrtf(2.0 / n_size);
        }
        sum = 0.0;
        for (int n = 0; n < n_size; n++) {
            sum += input[n] * cosf(M_PI * (2.0 * n + 1.0) * k / (2.0 * n_size));
        }
        output[k] = alpha * sum;
    }
}

int main(int argc, char *argv[]) {

    
    if (argc < 2) {
        printf("Uso: %s <tamaño del vector> [<seed>]\n", argv[0]);
        return EXIT_FAILURE;
    }  

    int n = atoi(argv[1]);

    _Float16 *input_small = (_Float16 *)malloc(N_SMALL * sizeof(_Float16));
    _Float16 *output_small = (_Float16 *)malloc(N_SMALL * sizeof(_Float16));

    // Se usa una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);


    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < N_SMALL; i++) {

        float input_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        input_small[i] = (_Float16)input_temp;

    }

    printf("Array input_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)input_small[i]);
    }
    printf("]\n");

    // Se ejecuta la operación DCT
    dct(input_small, output_small, N_SMALL);

    printf("Resultado DCT_small: [");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)output_small[i]);
    }
    printf("]\n");

    free(input_small);
    free(output_small);


    _Float16 *input = (_Float16 *)malloc(n * sizeof(_Float16));
    _Float16 *output = (_Float16 *)malloc(n * sizeof(_Float16));

    for (int i = 0; i < n; i++) {
        float input_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        input[i] = (_Float16)input_temp;
    }

    printf("Array input: [ ");
    for (int i = 0; i < n; i++) {
        printf("%f ", (float)input[i]);
    }
    printf("]\n");
   

    //Para medir el tiempo de ejecución
    
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    
    /* 
        Código del programa cuyo tiempo quiero medir
    */

    // Se ejecuta la operación DCT
    dct(input, output, n);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);

    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f %.10e\n", (float)output[n-1], (float)output[n-1]);

    free(input);
    free(output);

    return EXIT_SUCCESS;
}
