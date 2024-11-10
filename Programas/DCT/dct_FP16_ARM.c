//Declaracion de la constante M_PI con valor pi
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <arm_fp16.h>


// Definir el tamaño de la señal
#define N_SMALL 5


int main(int argc, char *argv[]) {

    
    if (argc < 2) {
        printf("Uso: %s <tamaño del vector> [<seed>]\n", argv[0]);
        return EXIT_FAILURE;
    }  

    int N = atoi(argv[1]);

    __fp16 *input_small = (__fp16 *)malloc(N_SMALL * sizeof(__fp16));
    __fp16 *output_small = (__fp16 *)malloc(N_SMALL * sizeof(__fp16));

    // Se usa una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);


    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < N_SMALL; i++) {

        float input_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        input_small[i] = (__fp16)input_temp;

    }

    printf("Array input_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)input_small[i]);
    }
    printf("]\n");

    //dct(input_small, output_small);

    // Operacion dct
    __fp16 alpha;
    __fp16 sum;
    for (int k = 0; k < N_SMALL; k++) {
        if (k == 0) {
            alpha = sqrt(1.0 / N_SMALL);
        } else {
            alpha = sqrt(2.0 / N_SMALL);
        }
        sum = 0.0;
        for (int n = 0; n < N_SMALL; n++) {
            sum += input_small[n] * cos(M_PI * (2.0 * n + 1.0) * k / (2.0 * N_SMALL));
        }
        output_small[k] = alpha * sum;
    }


    printf("Resultado DCT_small: [");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)output_small[i]);
    }
    printf("]\n");

    free(input_small);
    free(output_small);


    __fp16 *input = (__fp16 *)malloc(N * sizeof(__fp16));
    __fp16 *output = (__fp16 *)malloc(N * sizeof(__fp16));

    for (int i = 0; i < N; i++) {
        float input_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        input[i] = (__fp16)input_temp;
    }

    printf("Array input: [ ");
    for (int i = 0; i < N; i++) {
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

    for (int k = 0; k < N; k++) {
        if (k == 0) {
            alpha = sqrt(1.0 / N);
        } else {
            alpha = sqrt(2.0 / N);
        }
        sum = 0.0;
        for (int n = 0; n < N; n++) {
            sum += input[n] * cos(M_PI * (2.0 * n + 1.0) * k / (2.0 * N));
        }
        output[k] = alpha * sum;
    }


    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);

    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f\n", (float)output[N-1]);

    free(input);
    free(output);


    return EXIT_SUCCESS;
}
