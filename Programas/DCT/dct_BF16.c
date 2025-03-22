#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <arm_bf16.h>

#define N_SMALL 5


void dct(__bf16 *input, __bf16 *output, int n_size) {
    const float pi = 3.1415926535f; // Literal float para PI
    // Precomputar raíces cuadradas
    const __bf16 sqrt1 = (__bf16) sqrtf(1.0f / n_size); 
    const __bf16 sqrt2 = (__bf16) sqrtf(2.0f / n_size);

    for (int k = 0; k < n_size; k++) {
        __bf16 alpha = (k == 0) ? sqrt1 : sqrt2;
        __bf16 sum = 0.0f;
        // Calcular theta_k y delta para la relación de recurrencia
        float theta_k = (pi * k) / (2.0f * n_size);
        float delta = (pi * k) / n_size; // 2*theta_k
        
        // Precomputar cos(delta) y sin(delta)
        float cos_delta = cosf(delta);
        float sin_delta = sinf(delta);
        
        // Inicializar para n=0
        float cos_angle = cosf(theta_k);
        float sin_angle = sinf(theta_k);
        sum += input[0] * cos_angle;
        
        // Bucle interno: calcular para n >= 1 usando recurrencia
        for (int n = 1; n < n_size; n++) {
            float new_cos = cos_angle * cos_delta - sin_angle * sin_delta;
            float new_sin = sin_angle * cos_delta + cos_angle * sin_delta;
            sum += input[n] * new_cos;
            cos_angle = new_cos;
            sin_angle = new_sin;
        }
        output[k] = alpha * sum;
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

    int n = atoi(argv[optind]);

    __bf16 *input_small = (__bf16 *)malloc(N_SMALL * sizeof(__bf16));
    __bf16 *output_small = (__bf16 *)malloc(N_SMALL * sizeof(__bf16));

    // Se usa una semilla proporcionada como argumento o una por defecto
    unsigned int seed = (optind + 1 < argc) ? (unsigned int)atoi(argv[optind + 1]) : (unsigned int)time(NULL);
    srand(seed);


    // Generar elementos aleatorios entre 0 y 10
    for (int i = 0; i < N_SMALL; i++) {

        float input_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        input_small[i] = (__bf16)input_temp;

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


    __bf16 *input = (__bf16 *)malloc(n * sizeof(__bf16));
    __bf16 *output = (__bf16 *)malloc(n * sizeof(__bf16));

    for (int i = 0; i < n; i++) {
        float input_temp = ((float)rand() / (float)(RAND_MAX)) * 10.0f;
        input[i] = (__bf16)input_temp;
    }

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

    if(verbose){
        printf("Resultados ejecucion: ");
        for(int i = 0; i < n; i++){
            printf("%.10e ", (float)output[i]);
        }
        printf("\n");
    }

    free(input);
    free(output);


    return EXIT_SUCCESS;
}
