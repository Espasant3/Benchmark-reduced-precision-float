#!/bin/bash

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"


### COMPILACION DEL PROGRAMA BASE

gcc-14 -Wall -g axpy_FP32.c -o axpy_FP32

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extension .o
arm-linux-gnueabihf-gcc -Wall axpy_FP32.c -o axpy_FP32.o

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
aarch64-linux-gnu-gcc -Wall axpy_FP32.c -o axpy_FP32.out


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA x86 (USA EL TIPO DE DATO _Float16)

gcc-14 -Wall -g axpy_FP16.c -o axpy_FP16

# Mi ordenador actual no soporta de manera nativa las operaciones con las instrucciones avx512fp16
# Compilar el programa optimizado con AVX512 (nativo)
#gcc-14 -Wall -g axpy_FP16.c -o axpy_FP16_native-base -mavx512fp16

# Compilar el programa optimizado con AVX512 y precisión estándar
#gcc-14 -Wall -g axpy_FP16.c -o axpy_FP16_avx512_precision -fexcess-precision=standard -mavx512fp16

# Compilar el programa con máxima optimización
#gcc-14 -Wall -g axpy_FP16.c -o axpy_FP16_max_performance -fexcess-precision=standard -mfpmath=sse -mavx512fp16

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extension .o
#arm-linux-gnueabihf-gcc -Wall axpy_FP16.c -o axpy_FP16.o

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
aarch64-linux-gnu-gcc -Wall axpy_FP16.c -o axpy_FP16.out


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (USA EL TIPO DE DATO __fp16)

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extension .o
# Al parecer no reconoce este tipo de dato para ARM de 32 bits el método que usamos para compilación cruzada
#arm-linux-gnueabihf-gcc -Wall axpy_FP16_ARM.c -o axpy_FP16_ARM.o

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
aarch64-linux-gnu-gcc -Wall axpy_FP16_ARM.c -o axpy_FP16_ARM.out


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (USA EL TIPO DE DATO __bf16)

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extension .o
# Al parecer no reconoce este tipo de dato para ARM de 32 bits el método que usamos para compilación cruzada
#arm-linux-gnueabihf-gcc -Wall axpy_BF16.c -o axpy_BF16.o

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
aarch64-linux-gnu-gcc -Wall axpy_BF16.c -o axpy_BF16.out

echo ""
