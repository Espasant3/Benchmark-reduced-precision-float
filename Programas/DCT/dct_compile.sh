#!/bin/bash

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

gcc-14 -Wall -g dct_FP32.c -o dct_FP32 -lm

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
aarch64-linux-gnu-gcc -Wall dct_FP32.c -o dct_FP32.out -lm

