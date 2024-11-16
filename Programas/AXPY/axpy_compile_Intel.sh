#!/bin/bash

# Inicializar variables
force_run=false

# Verificar si el primer argumento es --force
if [[ "$1" == "--force" ]]; then
    force_run=true
    shift
else
    # Procesar argumentos de entrada
    for arg in "$@"; do
        if [[ "$arg" == "--force" ]]; then
            force_run=true
            set -- "${@/--force/}"
            break
        fi
    done
fi

### SCRIPT DE COMPILACION PARA ARQUITECTURA INTEL x86

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"


### COMPILACION DEL PROGRAMA BASE

gcc-14 -Wall -g axpy_FP32.c -o axpy_FP32


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA x86 (USA EL TIPO DE DATO _Float16)

gcc-14 -Wall -g axpy_FP16.c -o axpy_FP16 -fexcess-precision=16


if grep -q "avx512fp16" /proc/cpuinfo; then
    
    echo "AVX512FP16 support detected. Compiling with -mavx512fp16."

    # Compilar el programa optimizado con AVX512 (nativo)
    gcc-14 -Wall -g axpy_FP16.c -o axpy_FP16_native-base -mavx512fp16
    # Compilar el programa optimizado con AVX512 y precisión estándar
    gcc-14 -Wall -g axpy_FP16.c -o axpy_FP16_avx512_precision -fexcess-precision=standard -mavx512fp16
    # Compilar el programa con máxima optimización
    gcc-14 -Wall -g axpy_FP16.c -o axpy_FP16_max_performance -fexcess-precision=standard -mfpmath=sse -mavx512fp16

else
    echo "AVX512FP16 not supported on this system. Skipping compilation with -mavx512fp16."
fi

# Compilación cruzada para ARM de 64 bits

if $force_run; then

    ### COMPILACION DEL PROGRAMA BASE

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall axpy_FP32.c -o axpy_FP32.out


    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA x86 (USA EL TIPO DE DATO _Float16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall axpy_FP16.c -o axpy_FP16.out


    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (USA EL TIPO DE DATO __fp16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall axpy_FP16_ARM.c -o axpy_FP16_ARM.out


    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (USA EL TIPO DE DATO __bf16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall axpy_BF16.c -o axpy_BF16.out

fi

exit 0