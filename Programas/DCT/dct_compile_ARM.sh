#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA ARM DE 64 BITS

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

OPT_FLAGS="-O3 -march=armv8.2-a+fp16+fp16fml+simd -ftree-vectorize"

COMMON_FLAGS="-Wall -lm"
### COMPILACION DEL PROGRAMA BASE

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS dct_FP32.c -o dct_FP32.out $OPT_FLAGS

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS -fexcess-precision=16 dct_FP16.c -o dct_FP16.out $OPT_FLAGS

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __fp16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS dct_FP16_ARM.c -o dct_FP16_ARM.out $OPT_FLAGS

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __bf16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS dct_BF16.c -o dct_BF16.out $OPT_FLAGS

exit 0