#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arm_fp16.h>

#define N_SMALL 5

// Función AXPY
void axpy(int n, __fp16 a, __fp16 *x, __fp16 *y) {
    for (int i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <tamaño del vector> [<seed>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    __fp16 a = 2.3752f16;
    __fp16 *x_small = (__fp16 *)malloc(N_SMALL * sizeof(__fp16));
    __fp16 *y_small = (__fp16 *)malloc(N_SMALL * sizeof(__fp16));

    if (x_small == NULL || y_small == NULL) {
        printf("Error al asignar memoria\n");
        return EXIT_FAILURE;
    }

    // Se usa una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);

    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < N_SMALL; i++) {
        float x_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0;
        float y_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0;
        x_small[i] = (__fp16)x_temp;
        y_small[i] = (__fp16)y_temp;
    }

    printf("Array x_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)x_small[i]);
    }
    printf("]\nArray y_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)y_small[i]);
    }

    // Se ejecuta la operación AXPY
    axpy(N_SMALL, a, x_small, y_small);

    printf("]\nArray y_small despues de AXPY: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", (float)y_small[i]);
    }
    printf("]\n");

    free(x_small);
    free(y_small);


    __fp16 *x = (__fp16 *)malloc(n * sizeof(__fp16));
    __fp16 *y = (__fp16 *)malloc(n * sizeof(__fp16));

    if (x == NULL || y == NULL) {
        printf("Error al asignar memoria\n");
        return EXIT_FAILURE;
    }

    
    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < n; i++) {
        float x_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        float y_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        x[i] = (__fp16)x_temp;
        y[i] = (__fp16)y_temp;
    }


    //Para medir el tiempo de ejecución
    
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    
    ////    Código del programa cuyo tiempo quiero medir
    
    // Se ejecuta la operación AXPY
    axpy(n, a, x, y);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);

    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f %.10e\n", (float)y[n-1], (float)y[n-1]);

    // Liberar memoria asignada
    free(x);
    free(y);

    return EXIT_SUCCESS;
}
