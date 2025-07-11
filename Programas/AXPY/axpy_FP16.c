#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N_SMALL 5

// Función AXPY
void axpy(int n, _Float16 a, _Float16 *x, _Float16 *y) {
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
    
    _Float16 a = 2.3752f16;
    _Float16 *x_small = (_Float16 *)malloc(N_SMALL * sizeof(_Float16));
    _Float16 *y_small = (_Float16 *)malloc(N_SMALL * sizeof(_Float16));
    
    // Verificar la asignación de memoria
    if (x_small == NULL || y_small == NULL) {
        printf("Error en la asignación de memoria\n");
        return EXIT_FAILURE;
    }

    // Se usa una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (optind + 1 < argc) ? (unsigned int)atoi(argv[optind + 1]) : (unsigned int)time(NULL);
    srand(seed);

    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < N_SMALL; i++) {

        float x_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        float y_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        x_small[i] = (_Float16)x_temp;
        y_small[i] = (_Float16)y_temp;

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

    _Float16 *x = (_Float16 *)malloc(n * sizeof(_Float16));
    _Float16 *y = (_Float16 *)malloc(n * sizeof(_Float16));

    if (x == NULL || y == NULL) {
        printf("Error al asignar memoria\n");
        return EXIT_FAILURE;
    }

    
    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < n; i++) {
        float x_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        float y_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        x[i] = (_Float16)x_temp;
        y[i] = (_Float16)y_temp;
    }


    if(verbose){
        printf("Datos ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", (float)x[i]);
        }
        printf("\n"); 
    }


    //Para medir el tiempo de ejecución
    
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    
    ////    Código del programa cuyo tiempo quiero medir
    
    axpy(n, a, x, y);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tiempo de ejecucion: %f\n", cpu_time_used);

    // Se imprime un valor al final para evitar que las optimizaciones se salten alguna operaciones

    printf("%f %.10e\n", (float)y[n-1], (float)y[n-1]);

    if(verbose){
        printf("Resultados ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", (float)y[i]);
        }
        printf("\n");
    }

    // Liberar memoria asignada
    free(x);
    free(y);

    return EXIT_SUCCESS;
}
