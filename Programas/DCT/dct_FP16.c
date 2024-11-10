//Declaracion de la constante M_PI con valor pi
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>


// Definir el tamaño de la señal
#define N_SMALL 5

void dct(_Float16 *input, _Float16 *output) {
    _Float16 alpha;
    _Float16 sum;
    for (int k = 0; k < N_SMALL; k++) {
        if (k == 0) {
            alpha = sqrt(1.0 / N_SMALL);
        } else {
            alpha = sqrt(2.0 / N_SMALL);
        }
        sum = 0.0;
        for (int n = 0; n < N_SMALL; n++) {
            sum += input[n] * cos(M_PI * (2.0 * n + 1.0) * k / (2.0 * N_SMALL));
        }
        output[k] = alpha * sum;
    }
}

int main(int argc, char *argv[]) {

    /**/
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

    dct(input_small, output_small);

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

    dct(input, output);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);

    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f\n", (float)output[n-1]);

    free(input);
    free(output);

    return EXIT_SUCCESS;
}
