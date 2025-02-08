#!/bin/bash

### SCRIPT DE COMPILACIÓN PARA HUAWEI ASCEND (CANN) CON OPTIMIZACIONES Y PRECISIÓN

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

# Flags de optimización
OPT_FLAGS="-march=armv8.2-a -mfpu=neon-fp16 -fexcess-precision=standard -ftree-vectorize -DFLOAT16"

# Compilar axpy_FP16_ARM.c
cann $OPT_FLAGS axpy_FP16_ARM.c -o axpy_FP16_ARM.out

# Compilar axpy_FP16.c
cann $OPT_FLAGS axpy_FP16.c -o axpy_FP16.out

# Compilar axpy_BF16.c
cann $OPT_FLAGS axpy_BF16.c -o axpy_BF16.out
