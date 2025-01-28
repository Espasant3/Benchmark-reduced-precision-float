#!/bin/bash

# Función para comprobar si qemu-arm está instalado
check_qemu_arm() {
    if command -v qemu-arm >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# Inicializar variables
force_run=false

# Procesar argumentos de entrada
while [[ "$1" =~ ^- && ! "$1" == "--" ]]; do case $1 in
  --force )
    force_run=true
    ;;
esac; shift; done
if [[ "$1" == '--' ]]; then shift; fi

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

# Comprobar si estamos en una arquitectura ARM de 32 bits
if [ "$(uname -m)" = "armv7l" ] || [ "$(uname -m)" = "armv8l" ]; then
    echo "Arquitectura ARM de 32 bits detectada. Ejecutando nativamente."
    for file in *.out; do
        if [ -f "$file" ] && [ -x "$file" ]; then
            if [ -z "$seed" ]; then
                echo "Ejecutando $file con N $tamanhoN"
                ./"$file" "$tamanhoN"
            else
                echo "Ejecutando $file con N $tamanhoN y seed $seed"
                ./"$file" "$tamanhoN" "$seed"
            fi
            echo ""
        fi
    done
else
    echo "No es arquitectura ARM de 32 bits. Comprobando qemu-arm..."
    if check_qemu_arm; then
        echo "qemu-arm detectado. Ejecutando con emulación."
        for file in *.o; do
            if [ -f "$file" ] && [ -x "$file" ]; then
                if [ -z "$seed" ]; then
                    echo "Ejecutando $file con N $tamanhoN"
                    qemu-arm ./"$file" "$tamanhoN"
                else
                    echo "Ejecutando $file con N $tamanhoN y seed $seed"
                    qemu-arm ./"$file" "$tamanhoN" "$seed"
                fi
                echo ""
            fi
        done
    else
        echo "qemu-arm no está instalado y no es una arquitectura ARM de 32 bits. No se pueden ejecutar los archivos."
    fi
fi
