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

# Verificar si se proporcionaron al menos un parámetro
if [ $# -lt 1 ]; then
    echo "Uso: $0 <tamanho N> [<seed>] [--force]"
    exit 1
fi

tamanhoN=$1
seed=$2

# Comprobar que tamanhoN sea un número positivo mayor que 0
if ! [[ "$tamanhoN" =~ ^[0-9]+$ ]] || [ "$tamanhoN" -le 0 ]; then
    echo "Error: tamanho N debe ser un número positivo mayor que 0."
    exit 1
fi

# Si se proporciona seed, comprobar que sea un número positivo mayor que 0
if [ -n "$seed" ]; then
    if ! [[ "$seed" =~ ^[0-9]+$ ]] || [ "$seed" -le 0 ]; then
        echo "Error: seed debe ser un número positivo mayor que 0 si se proporciona."
        exit 1
    fi
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

echo "Valor de force_flag: $force_flag"

# Dependiendo del proveedor del CPU, realizar diferentes acciones
case "$VENDOR" in
    GenuineIntel)
        CPU_VENDOR="intel"
        ;;
    AuthenticAMD)
        CPU_VENDOR="amd"
        ;;
    Loongson)
        CPU_VENDOR="loongson"
        ;;
    MIPS)
        CPU_VENDOR="mips"
        ;;
    HiSilicon)
        CPU_VENDOR="huawei"
        ;;
    riscv)
        CPU_VENDOR="riscv"
        ;;
    power)
        CPU_VENDOR="power_isa"
        ;;
    *)
        CPU_VENDOR="unknown"
        ;;
esac

# Dependiendo de la combinación de arquitectura y proveedor, realizar diferentes acciones
case "$ARCH" in
    x86_64)
        case "$CPU_VENDOR" in
            intel)
                echo "Arquitectura: Intel de 64 bits"
                # Instrucciones específicas para Intel de 64 bits
                echo "Compilando AXPY"
                ./AXPY/axpy_run_Intel.sh $tamanhoN $seed $force_flag
                echo "Compilando DCT"
                ./DCT/dct_run_Intel.sh $tamanhoN $seed $force_flag
                echo "Compilando DWT_1D"
                #./DWT_1D/dwt_1d_run_Intel.sh $tamanhoN $seed $force_flag
                echo "Compilando PCA"
                #./PCA/pca_run_Intel.sh $tamanhoN $seed $force_flag
                ;;
            amd)
                echo "Arquitectura: AMD de 64 bits"
                # Instrucciones específicas para AMD de 64 bits
                echo "Compilando AXPY"
                ./AXPY/axpy_run_AMD.sh $tamanhoN $seed $force_flag
                echo "Compilando DCT"
                ./DCT/dct_run_AMD.sh $tamanhoN $seed $force_flag
                echo "Compilando DWT_1D"
                ./DWT_1D/dwt_1d_run_AMD.sh $tamanhoN $seed $force_flag
                echo "Compilando PCA"
                ./PCA/pca_run_AMD.sh $tamanhoN $seed $force_flag
                ;;
            *)
                echo "Vendor desconocido para x86_64"
                ;;
        esac
        ;;
    aarch64)
        echo "Arquitectura: ARM de 64 bits"
        case "$CPU_VENDOR" in
            huawei)
                # Detecta por igual Huawei Ascend y HiSilicon Kunpeng
                echo "Arquitectura: Huawei de 64 bits"
                echo "No hay soporte oficial por el momento."
                ;;
            *)
                echo "Compilando AXPY"
                ./AXPY/axpy_run_ARM.sh $tamanhoN $seed $force_flag
                echo "Compilando DCT"
                ./DCT/dct_run_ARM.sh $tamanhoN $seed $force_flag
                echo "Compilando DWT_1D"
                ./DWT_1D/dwt_1d_run_ARM.sh $tamanhoN $seed $force_flag
                echo "Compilando PCA"
                ./PCA/pca_run_ARM.sh $tamanhoN $seed $force_flag
                ;;
        esac
        ;;
    mips|mips64)
        echo "Arquitectura: MIPS"
        case "$CPU_VENDOR" in
            loongson)
                echo "Arquitectura: Loongson basada en MIPS"
                echo "No hay soporte oficial por el momento."
                ;;
            mips)
                echo "Arquitectura: MIPS genérica"
                # Instrucciones específicas para MIPS
                echo "No hay soporte oficial por el momento"
                ;;
            *)
                echo "Vendor desconocido para MIPS"
                ;;
        esac
        ;;
    riscv|riscv64)
        echo "Arquitectura: RISC-V"
        case "$CPU_VENDOR" in
            loongson)
                echo "Arquitectura: Loongson basada en RISC-V"
                echo "No hay soporte oficial por el momento."
                ;;
            riscv)
                echo "Arquitectura: RISC-V de 64 bits"
                echo "No hay soporte oficial por el momento."
                ;;
            *)
                echo "Vendor desconocido para RISC-V"
                ;;
        esac
        ;;
    powerpc|ppc64|ppc64le)
        echo "Arquitectura: Power-ISA"
        echo "No hay soporte oficial por el momento."
        ;;
    loongarch64)
        echo "Arquitectura: LoongArch de 64 bits"
        echo "No hay soporte oficial por el momento."
        ;;
    *)
        echo "Arquitectura desconocida o no soportada: $ARCH"
        ;;
esac

exit 0