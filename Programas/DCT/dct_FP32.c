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

void dct(float *input, float *output) {
    float alpha;
    float sum;
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

    float *input_small = (float *)malloc(N_SMALL * sizeof(float));
    float *output_small = (float *)malloc(N_SMALL * sizeof(float));

    // Se usa una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);

    for (int i = 0; i < N_SMALL; i++) {
        input_small[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
    }

    printf("Array input_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", input_small[i]);
    }
    printf("]\n");

    dct(input_small, output_small);

    printf("Resultado DCT_small: [");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", output_small[i]);
    }
    printf("]\n");

    free(input_small);
    free(output_small);


    float *input = (float *)malloc(n * sizeof(float));
    float *output = (float *)malloc(n * sizeof(float));

    for (int i = 0; i < n; i++) {
        input[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
    }

    printf("Array input: [ ");
    for (int i = 0; i < n; i++) {
        printf("%f ", input[i]);
    }
    printf("]\n");

    dct(input, output);

    printf("Resultado DCT: [");
    for (int i = 0; i < n; i++) {
        printf("%f ", output[i]);
    }
    printf("]\n");

    free(input);
    free(output);

    return EXIT_SUCCESS;
}
