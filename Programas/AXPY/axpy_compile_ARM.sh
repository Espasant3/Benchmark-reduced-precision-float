#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA ARM DE 64 BITS

# Inicializar variables
force_run=false
additional_flags=""

# Uso: $0 [--force] [opciones adicionales]
usage() {
    echo "Uso: $0 [-f|--force] [opciones adicionales]"
    exit 1
}

# Procesar argumentos manualmente
while [[ $# -gt 0 ]]; do
    case "$1" in
        -f|--force)
            force_run=true
            shift
            ;;
        --)  # Fin de las opciones
            shift
            break
            ;;
        -*)
            # Flags adicionales para el compilador
            additional_flags+=" $1"
            echo "Flag adicional añadido para compilar: $1"
            shift
            ;;
        *)
            # Argumentos posicionales (tamaño N, seed, etc.)
            break
            ;;
    esac
done

COMMON_FLAGS="-Wall"

OPT_FLAGS="-O3 -march=armv8.2-a+fp16+fp16fml+simd -ftree-vectorize -fomit-frame-pointer -fPIC $additional_flags"

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"


### COMPILACION DEL PROGRAMA BASE

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS axpy_FP32.c -o axpy_FP32.out $OPT_FLAGS


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS -fexcess-precision=16 axpy_FP16.c -o axpy_FP16.out $OPT_FLAGS
### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __fp16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS axpy_FP16_ARM.c -o axpy_FP16_ARM.out $OPT_FLAGS

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __bf16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS axpy_BF16.c -o axpy_BF16.out $OPT_FLAGS

exit 0