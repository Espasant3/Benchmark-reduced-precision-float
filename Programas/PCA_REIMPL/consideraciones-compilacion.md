
## Consideraciones para la compilación de los programas

1. El programa `pca_reimpl_FP32.c` es exactamente igual que el programa `pca_FP32.c` puesto que no es necesario reimplementar en float la función `lapacke_ssyev` ya que esta ya opera con este tipo de dato.
2. Compilar con el flag `-lm`.
3. Para las variables globales de FP16 y BF16 se ha usado los valores de las macros de gcc obtenidas mediante: 
```bash
{ echo | gcc -dM -E - | grep '^#define __FLT16_'; echo | gcc -dM -E - | grep '^#define __BFLT16_'; }
```
