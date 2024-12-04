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

OPT_FLAGS="-march=icelake-client -mtune=icelake-client -O3 -fomit-frame-pointer -fPIC"

COMMON_FLAGS="-Wall -g -lm"

### SCRIPT DE COMPILACION PARA ARQUITECTURA AMD x86_64

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

### COMPILACION DEL PROGRAMA BASE

gcc-14 $COMMON_FLAGS dct_FP32.c -o dct_FP32


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

if grep -q "sse2" /proc/cpuinfo; then
    echo "SSE2 support detected. Compiling programs with _Float16 data."
    
    gcc-14 $COMMON_FLAGS dct_FP16.c -o dct_FP16 -fexcess-precision=16 $OPT_FLAGS

    # Para los futuros procesadores AMD con arquitectura Zen 6

    if grep -q "avx512fp16" /proc/cpuinfo; then
        echo "AVX512FP16 support detected. Compiling with -mavx512fp16."
        
        COMMON_FLAGS+=" -march=znver6 -mtune=znver6 -mavx512fp16"

        # Compilar el programa optimizado con AVX512-FP16 (nativo para Zen 6)
        gcc-14 $COMMON_FLAGS dct_FP16.c -o dct_FP16_native-base $OPT_FLAGS
        # Compilar el programa optimizado con AVX512-FP16 y precisión estándar
        gcc-14 $COMMON_FLAGS dct_FP16.c -o dct_FP16_avx512fp16_precision -fexcess-precision=16 $OPT_FLAGS
        # Compilar el programa con máxima optimización para AVX512-FP16
        gcc-14 $COMMON_FLAGS dct_FP16.c -o dct_FP16_max_performance -fexcess-precision=16 -mfpmath=sse $OPT_FLAGS

        # Eliminar los flags específicos de esta sección
        COMMON_FLAGS="${COMMON_FLAGS/-mavx512fp16/}"
        COMMON_FLAGS="${COMMON_FLAGS/-march=znver6/}"
        COMMON_FLAGS="${COMMON_FLAGS/-mtune=znver6/}"
        COMMON_FLAGS=$(echo $COMMON_FLAGS | tr -s ' ' | xargs)

    elif grep -q "avx512f" /proc/cpuinfo; then
        echo "AVX512 support detected. Compiling with -mavx512f."

        # Compilar el programa optimizado con AVX512 (nativo para Zen 4 y Zen 5)
        gcc-14 -Wall -g dct_FP16.c -o dct_FP16_native-base -mavx512f -lm
        # Compilar el programa optimizado con AVX512 y precisión estándar
        gcc-14 -Wall -g dct_FP16.c -o dct_FP16_avx512_precision -fexcess-precision=16 -mavx512f -lm
        # Compilar el programa con máxima optimización para AVX512
        gcc-14 -Wall -g dct_FP16.c -o dct_FP16_max_performance -fexcess-precision=16 -mfpmath=sse -mavx512f -lm

    else
        echo "AVX512 not supported on this system. Skipping compilation with AVX512 flags."
    fi
else
    echo "SSE2 not supported on this system. Skipping compilation for programs with _Float16."
fi


if $force_run; then

    echo "Flag --force detectada. Cross-compilando programas para arquitectura ARM."
    ### COMPILACION DEL PROGRAMA BASE

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall dct_FP32.c -o dct_FP32.out -lm

    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall -fexcess-precision=16 dct_FP16.c -o dct_FP16.out -lm

    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __fp16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall -mfp16-format=ieee dct_FP16_ARM.c -o dct_FP16_ARM.out -lm

    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __bf16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall dct_BF16.c -o dct_BF16.out -lm

fi

exit 0