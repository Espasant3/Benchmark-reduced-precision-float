#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA AMD x86_64

# Inicializar variables
force_run=false
additional_flags=""

# Uso: $0 [--force] [opciones adicionales]
usage() {
    # Mostrar ayuda de uso del script
    echo "Uso: $0 [-f|--force] [opciones adicionales]"
    echo "  -f, --force       Fuerza la compilación cruzada de todos los programas a la arquitectura aarch64."
    echo "  -h, --help        Muestra esta ayuda y sale."
    exit 0
}

# Procesar argumentos manualmente
while [[ $# -gt 0 ]]; do
    case "$1" in
        -f|--force)
            force_run=true
            shift
            ;;
        -h|--help)
            usage
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

COMMON_FLAGS="-Wall -g"

OPT_FLAGS="-O3 -fomit-frame-pointer $additional_flags"

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

### COMPILACION DEL PROGRAMA BASE

gcc-14 $COMMON_FLAGS axpy_FP32.c -o axpy_FP32


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

if grep -q "sse2" /proc/cpuinfo; then
    echo "SSE2 support detected. Compiling programs with _Float16 data."

    gcc-14 $COMMON_FLAGS axpy_FP16.c -o axpy_FP16 -fexcess-precision=16 $OPT_FLAGS

    # Para los futuros procesadores AMD con arquitectura Zen 6

    if grep -q "avx512fp16" /proc/cpuinfo; then
        echo "AVX512FP16 support detected. Compiling with -mavx512fp16."

        COMMON_FLAGS+=" -march=znver6 -mtune=znver6 -mavx512fp16"

        # Compilar el programa optimizado con AVX512-FP16 (nativo para Zen 6)
        gcc-14 $COMMON_FLAGS axpy_FP16.c -o axpy_FP16_native-base $OPT_FLAGS 
        # Compilar el programa optimizado con AVX512-FP16 y precisión estándar
        gcc-14 $COMMON_FLAGS axpy_FP16.c -o axpy_FP16_avx512fp16_precision -fexcess-precision=16 $OPT_FLAGS
        # Compilar el programa con máxima optimización para AVX512-FP16
        gcc-14 $COMMON_FLAGS axpy_FP16.c -o axpy_FP16_max_performance -fexcess-precision=16 -mfpmath=sse $OPT_FLAGS


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

    echo "Flag [-f]--force detectada. Cross-compilando programas para arquitectura ARM."
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