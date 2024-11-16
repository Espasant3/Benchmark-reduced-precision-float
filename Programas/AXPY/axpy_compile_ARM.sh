#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA ARM DE 64 BITS

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

### COMPILACION DEL PROGRAMA BASE

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc -Wall axpy_FP32.c -o axpy_FP32.out


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (USA EL TIPO DE DATO __fp16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc -Wall axpy_FP16_ARM.c -o axpy_FP16_ARM.out

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (USA EL TIPO DE DATO __bf16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc -Wall axpy_BF16.c -o axpy_BF16.out

exit 0