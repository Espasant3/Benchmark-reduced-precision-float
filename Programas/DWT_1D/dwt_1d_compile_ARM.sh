#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA ARM DE 64 BITS

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

### COMPILACION DEL PROGRAMA BASE

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 -Wall dwt_1d_FP32.c -o dwt_1d_FP32.out


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
#gcc-14 -Wall -fexcess-precision=16 dwt_1d_FP16.c -o dwt_1d_FP16.out

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __fp16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
#gcc-14 -Wall dwt_1d_FP16_ARM.c -o dwt_1d_FP16_ARM.out

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __bf16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
#gcc-14 -Wall dwt_1d_BF16.c -o dwt_1d_BF16.out

exit 0