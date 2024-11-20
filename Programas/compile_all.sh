#!/bin/bash

# Inicializar variables
force_run=false
force_flag=""

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

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

# This script checks the CPU architecture and vendor.
# The values for `uname -m` can be found at: https://wiki.debian.org/ArchitectureSpecificsMemo

# Obtener la arquitectura del sistema
ARCH=$(uname -m)

# Obtener el proveedor del CPU
VENDOR=$(grep -m 1 'vendor_id' /proc/cpuinfo | awk '{print $3}')
# Asignar valor a force_flag basado en force_run
if [ "$force_run" = true ]; then
    force_flag="--force"
fi

# Dependiendo del proveedor del CPU, realizar diferentes acciones
case "$VENDOR" in
    GenuineIntel)
        CPU_VENDOR="intel"
        ;;
    AuthenticAMD)
        CPU_VENDOR="amd"
        ;;
    HiSilicon)
        CPU_VENDOR="huawei"
        ;;
    *)
        CPU_VENDOR="unknown"
        ;;
esac

# Dependiendo de la combinación de arquitectura y proveedor, realizar diferentes acciones
case "$ARCH" in
    x86_64|amd64|x64)
        case "$CPU_VENDOR" in
            intel)
                echo "Arquitectura: Intel de 64 bits"
                # Instrucciones específicas para Intel de 64 bits
                echo "Compilando AXPY"
                ./AXPY/axpy_compile_Intel.sh $force_flag
                echo "Compilando DCT"
                ./DCT/dct_compile_Intel.sh $force_flag
                echo "Compilando DWT_1D"
                #./DWT_1D/dwt_1d_compile_Intel.sh $force_flag
                echo "Compilando PCA"
                #./PCA/pca_compile_Intel.sh $force_flag
                ;;
            amd)
                echo "Arquitectura: AMD de 64 bits"
                # Instrucciones específicas para AMD de 64 bits
                echo "Compilando AXPY"
                ./AXPY/axpy_compile_AMD.sh $force_flag
                echo "Compilando DCT"
                ./DCT/dct_compile_AMD.sh $force_flag
                echo "Compilando DWT_1D"
                ./DWT_1D/dwt_1d_compile_AMD.sh $force_flag
                echo "Compilando PCA"
                ./PCA/pca_compile_AMD.sh $force_flag
                ;;
            *)
                echo "Vendor desconocido para x86_64"
                ;;
        esac
        ;;
    aarch64|arm64)
        echo "Arquitectura: ARM de 64 bits"
        case "$CPU_VENDOR" in
            huawei)
                # Detecta por igual Huawei Ascend y HiSilicon Kunpeng
                echo "Arquitectura: Huawei de 64 bits"
                echo "No hay soporte oficial por el momento."
                ;;
            *)
                echo "Compilando AXPY"
                ./AXPY/axpy_compile_ARM.sh $force_flag
                echo "Compilando DCT"
                ./DCT/dct_compile_ARM.sh $force_flag
                echo "Compilando DWT_1D"
                ./DWT_1D/dwt_1d_compile_ARM.sh $force_flag
                echo "Compilando PCA"
                ./PCA/pca_compile_ARM.sh $force_flag
                ;;
        esac
        ;;
    *)
        echo "Arquitectura desconocida o no soportada: $ARCH"
        ;;
esac

exit 0