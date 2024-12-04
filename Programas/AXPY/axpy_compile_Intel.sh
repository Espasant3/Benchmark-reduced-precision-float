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

COMMON_FLAGS="-Wall -g"

### SCRIPT DE COMPILACION PARA ARQUITECTURA INTEL x86

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"


### COMPILACION DEL PROGRAMA BASE

gcc-14 $COMMON_FLAGS axpy_FP32.c -o axpy_FP32 $OPT_FLAGS


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

if grep -q "sse2" /proc/cpuinfo; then
    echo "SSE2 support detected. Compiling programs with _Float16 data."

    gcc-14 $COMMON_FLAGS axpy_FP16.c -o axpy_FP16 -fexcess-precision=16 $OPT_FLAGS

    if grep -q "avx512fp16" /proc/cpuinfo; then
        
        echo "AVX512FP16 support detected. Compiling with -mavx512fp16."
        COMMON_FLAGS+=" -mavx512fp16"

        # Compilar el programa optimizado con AVX512 (nativo)
        gcc-14 $COMMON_FLAGS axpy_FP16.c -o axpy_FP16_native-base $OPT_FLAGS
        # Compilar el programa optimizado con AVX512 y precisión de 16 bits
        gcc-14 $COMMON_FLAGS axpy_FP16.c -o axpy_FP16_avx512_precision -fexcess-precision=16 $OPT_FLAGS
        # Compilar el programa con máxima optimización
        gcc-14 $COMMON_FLAGS axpy_FP16.c -o axpy_FP16_max_performance -fexcess-precision=16 -mfpmath=sse $OPT_FLAGS

        # Eliminar los flags específicos de esta sección
        COMMON_FLAGS="${COMMON_FLAGS/-mavx512fp16/}" 
        COMMON_FLAGS=$(echo $COMMON_FLAGS | tr -s ' ' | xargs)

    else
        echo "AVX512FP16 not supported on this system. Skipping compilation with -mavx512fp16."
    fi

else
    echo "SSE2 not supported on this system. Skipping compilation for programs with _Float16."
fi


# Compilación cruzada para ARM de 64 bits

if $force_run; then

    echo "Flag --force detectada. Cross-compilando programas para arquitectura ARM."
    ### COMPILACION DEL PROGRAMA BASE

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall axpy_FP32.c -o axpy_FP32.out


    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall -fexcess-precision=16 axpy_FP16.c -o axpy_FP16.out


    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __fp16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall axpy_FP16_ARM.c -o axpy_FP16_ARM.out


    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __bf16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall axpy_BF16.c -o axpy_BF16.out

fi

exit 0