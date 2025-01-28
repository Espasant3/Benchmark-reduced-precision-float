#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA ARM DE 32 BITS

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

### COMPILACION DEL PROGRAMA BASE

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extensión .o
arm-linux-gnueabihf-gcc -Wall program_FP32.c -o program_FP32.o

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extensión .o
arm-linux-gnueabihf-gcc -Wall -mfp16-format=ieee program_FP16.c -o program_FP16.o

### COMPILACION DEL PROGRAMA CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (USA EL TIPO DE DATO __fp16)

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extensión .o
arm-linux-gnueabihf-gcc -Wall -mfp16-format=ieee program_FP16_ARM.c -o program_FP16_ARM.o

### COMPILACION DEL PROGRAMA CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (USA EL TIPO DE DATO __bf16)

# Compila para ARM de 32 bits, como distintivo el archivo tiene la extensión .o
arm-linux-gnueabihf-gcc -Wall -mfp16-format=ieee program_BF16.c -o program_BF16.o
