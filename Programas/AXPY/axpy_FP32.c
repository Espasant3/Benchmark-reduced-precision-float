#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N_SMALL 5

// Función AXPY
void axpy(int n, float a, float *x, float *y) {
    for (int i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
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

    float a = 2.3752f;
    float *x_small = (float *)malloc(N_SMALL * sizeof(float));
    float *y_small = (float *)malloc(N_SMALL * sizeof(float));

    if (x_small == NULL || y_small == NULL) {
        printf("Error al asignar memoria\n");
        return EXIT_FAILURE;
    }

    // Se usa una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (optind + 1 < argc) ? (unsigned int)atoi(argv[optind + 1]) : (unsigned int)time(NULL);
    srand(seed);

    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < N_SMALL; i++) {
        x_small[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
        y_small[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
    }

    printf("Array x_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", x_small[i]);
    }
    printf("]\nArray y_small: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", y_small[i]);
    }

    // Se ejecuta la operación AXPY
    axpy(N_SMALL, a, x_small, y_small);

    printf("]\nArray y_small despues de AXPY: [ ");
    for (int i = 0; i < N_SMALL; i++) {
        printf("%f ", y_small[i]);
    }
    printf("]\n");

    free(x_small);
    free(y_small);

    float *x = (float *)malloc(n * sizeof(float));
    float *y = (float *)malloc(n * sizeof(float));

    if (x == NULL || y == NULL) {
        printf("Error al asignar memoria\n");
        return EXIT_FAILURE;
    }

    
    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < n; i++) {
        x[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
        y[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
    }

    if(verbose){
        printf("Datos ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", x[i]);
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
    axpy(n, a, x, y);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);

    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f %.10e\n", y[n-1], y[n-1]);

    if(verbose){
        printf("Resultados ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", y[i]);
        }
        printf("\n");    
    }
    // Liberar memoria asignada
    free(x);
    free(y);

    
    return EXIT_SUCCESS;
}
