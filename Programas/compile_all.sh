#!/bin/bash

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
                ./AXPY/axpy_compile_Intel.sh
                echo "Compilando DCT"
                ./DCT/dct_compile_Intel.sh
                #echo "Compilando DWT_1D"
                #./DWT_1D/dwt_1d_compile_Intel.sh
                #echo "Compilando PCA"
                #./PCA/pca_compile_Intel.sh
                ;;
            amd)
                echo "Arquitectura: AMD de 64 bits"
                # Instrucciones específicas para AMD de 64 bits
                echo "Compilando AXPY"
                ./AXPY/axpy_compile_AMD.sh
                echo "Compilando DCT"
                ./DCT/dct_compile_AMD.sh
                echo "Compilando DWT_1D"
                ./DWT_1D/dwt_1d_compile_AMD.sh
                echo "Compilando PCA"
                ./PCA/pca_compile_AMD.sh
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
                ./AXPY/axpy_compile_ARM.sh
                echo "Compilando DCT"
                ./DCT/dct_compile_ARM.sh
                echo "Compilando DWT_1D"
                ./DWT_1D/dwt_1d_compile_ARM.sh
                echo "Compilando PCA"
                ./PCA/pca_compile_ARM.sh
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
        # No hay soporte oficial por el momento
        ;;
    loongarch64)
        echo "Arquitectura: LoongArch de 64 bits"
        # No hay soporte oficial por el momento
        ;;
    *)
        echo "Arquitectura desconocida o no soportada: $ARCH"
        ;;
esac
