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
#define N 8

void dct(float *input, float *output) {
    float alpha;
    float sum;
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
}

int main(int argc, char *argv[]) {

    /*
    if (argc < 2) {
        printf("Uso: %s <tamaño del vector> [<seed>]\n", argv[0]);
        return EXIT_FAILURE;
    }  

    int n = atoi(argv[1]);
    */

    float *input = (float *)malloc(N * sizeof(float));
    float *output = (float *)malloc(N * sizeof(float));

    input[0]=1.0f; input[1]=2.0f; input[2]=3.0f; input[3]=4.0f; input[4]=4.0f; input[5]=3.0f; input[6]=2.0f; input[7]=1.0f;

    /*
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);

    for (int i = 0; i < n; i++) {
        input[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
    }
    */

    dct(input, output);

    printf("DCT:\n");
    for (int i = 0; i < N; i++) {
        printf("%f\n", output[i]);
    }

    free(input);
    free(output);

    return EXIT_SUCCESS;
}
