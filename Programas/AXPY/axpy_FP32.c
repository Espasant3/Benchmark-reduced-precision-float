#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Función AXPY
void axpy(int n, float a, float *x, float *y) {
    for (int i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <tamaño del vector> [<semilla>]\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    float a = 2.3752f;
    float *x = (float *)malloc(n * sizeof(float));
    float *y = (float *)malloc(n * sizeof(float));

    if (x == NULL || y == NULL) {
        printf("Error al asignar memoria\n");
        return 1;
    }

    // Usar una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (argc > 2) ? atoi(argv[2]) : (unsigned int)time(NULL);
    srand(seed);

    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < n; i++) {
        x[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
        y[i] = ((float)rand() / (float)(RAND_MAX)) * 10.0;
    }

    printf("Array x:\n");
    for (int i = 0; i < n; i++) {
        printf("x[%d] = %f\n", i, x[i]);
    }

    printf("\nArray y antes de AXPY:\n");
    for (int i = 0; i < n; i++) {
        printf("y[%d] = %f\n", i, y[i]);
    }

    // Llamar a la función AXPY
    axpy(n, a, x, y);

    printf("\nArray y después de AXPY:\n");
    for (int i = 0; i < n; i++) {
        printf("y[%d] = %f\n", i, y[i]);
    }

    // Liberar memoria asignada
    free(x);
    free(y);

    return 0;
}
