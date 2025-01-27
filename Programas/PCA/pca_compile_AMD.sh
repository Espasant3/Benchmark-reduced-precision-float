#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA AMD x86_64

# Inicializar variables
force_run=false
additional_flags=""

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

# Procesar flags adicionales
for arg in "$@"; do
    if [[ "$arg" == -* && "$arg" != --* ]]; then
        additional_flags+=" $arg"
    else
        echo "Flag no válida: $arg"
        exit 1
    fi
done

COMMON_FLAGS="-Wall -g"

OPT_FLAGS="-march=icelake-client -mtune=icelake-client -O3 -fomit-frame-pointer -fPIC $additional_flags"

LINK_FLAGS="-lm -llapacke -llapack -lblas"

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

### COMPILACION DEL PROGRAMA BASE

gcc-14 $COMMON_FLAGS pca_FP32.c -o pca_FP32 $OPT_FLAGS $LINK_FLAGS


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

if grep -q "sse2" /proc/cpuinfo; then
    echo "SSE2 support detected. Compiling programs with _Float16 data."

    gcc-14 $COMMON_FLAGS pca_FP16.c -o pca_FP16 -fexcess-precision=16 $OPT_FLAGS $LINK_FLAGS

    # Para los futuros procesadores AMD con arquitectura Zen 6

    if grep -q "avx512fp16" /proc/cpuinfo; then
        echo "AVX512FP16 support detected. Compiling with -mavx512fp16."

        COMMON_FLAGS+=" -march=znver6 -mtune=znver6 -mavx512fp16"

        # Compilar el programa optimizado con AVX512-FP16 (nativo para Zen 6)
        gcc-14 $COMMON_FLAGS pca_FP16.c -o pca_FP16_native-base $OPT_FLAGS $LINK_FLAGS
        # Compilar el programa optimizado con AVX512-FP16 y precisión estándar
        gcc-14 $COMMON_FLAGS pca_FP16.c -o pca_FP16_avx512fp16_precision -fexcess-precision=16 $OPT_FLAGS $LINK_FLAGS
        # Compilar el programa con máxima optimización para AVX512-FP16
        gcc-14 $COMMON_FLAGS pca_FP16.c -o pca_FP16_max_performance -fexcess-precision=16 -mfpmath=sse $OPT_FLAGS $LINK_FLAGS


        # Eliminar los flags específicos de esta sección 
        COMMON_FLAGS="${COMMON_FLAGS/-march=znver6/}"
        COMMON_FLAGS="${COMMON_FLAGS/-mtune=znver6/}"
        COMMON_FLAGS="${COMMON_FLAGS/-mavx512fp16/}"
        COMMON_FLAGS=$(echo $COMMON_FLAGS | tr -s ' ' | xargs)

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
    aarch64-linux-gnu-gcc -Wall pca_FP32.c -o pca_FP32.out $LINK_FLAGS

    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall -fexcess-precision=16 pca_FP16.c -o pca_FP16.out $LINK_FLAGS

    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __fp16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall pca_FP16_ARM.c -o pca_FP16_ARM.out $LINK_FLAGS

    ### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __bf16)

    # Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
    aarch64-linux-gnu-gcc -Wall pca_BF16.c -o pca_BF16.out $LINK_FLAGS

fi

exit 0