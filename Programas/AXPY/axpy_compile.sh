#!/bin/bash

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

gcc-14 -Wall -g axpy_FP32.c -o axpy_FP32

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extension .o
arm-linux-gnueabihf-gcc axpy_FP32.c -o axpy_FP32.o -Wall

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
aarch64-linux-gnu-gcc axpy_FP32.c -o axpy_FP32.out -Wall

echo ""
