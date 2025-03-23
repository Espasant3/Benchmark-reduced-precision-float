#!/bin/bash

# Función para comprobar si qemu-aarch64 está instalado
check_qemu() {
    if command -v qemu-aarch64 >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# Inicializar variables
force_run=false

tamanhoN=""
seed=""

# Uso: $0 [-f|--force] <tamanho N> [<seed>]
usage() {
    echo "Uso: $0 [-f|--force] <tamanho N> [<seed>]"
    exit 1
}

# Procesar argumentos con GNU getopt
TEMP=$(getopt -o f --long force -n "$0" -- "$@")

# Verificar si hubo error en getopt
if [ $? != 0 ]; then
    echo "Error: Opción no reconocida o falta de argumento."
    usage
fi

eval set -- "$TEMP"

# Asignar variables basadas en opciones
while true; do
    case "$1" in
        -f|--force)
            force_run=true
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Error interno en getopt"
            exit 1
            ;;
    esac
done

# Verificar si se proporcionaron al menos un parámetro posicional (tamanhoN)
if [ $# -lt 1 ]; then
    usage
fi

# Asignar argumentos posicionales
tamanhoN=$1
seed=${2:-}

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

# Ejecutar todos los archivos sin extensión en el directorio actual una vez, ignorando .sh
for file in *; do
    if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
        if [ -z "$seed" ]; then
            echo "Ejecutando $file con N=$tamanhoN"
            ./"$file" "$tamanhoN"
        else
            echo "Ejecutando $file con N=$tamanhoN y seed=$seed"
            ./"$file" "$tamanhoN" "$seed"
        fi
        echo ""
    fi
done


# Ejecutar solo si el flag --force está presente
if $force_run; then
    echo "Flag [-f]--force presente. Intentando ejecutar todos los archivos con extensión .out en el directorio actual."
    echo "Comprobando qemu-aarch64..."
    if check_qemu; then
        echo "qemu-aarch64 detectado. Ejecutando con emulación."
        for file in *.out; do
            if [ -f "$file" ] && [ -x "$file" ]; then
                if [ -z "$seed" ]; then
                    echo "Ejecutando $file con N=$tamanhoN"
                    qemu-aarch64 ./"$file" "$tamanhoN"
                else
                    echo "Ejecutando $file con N=$tamanhoN y seed=$seed"
                    qemu-aarch64 ./"$file" "$tamanhoN" "$seed"
                fi
                echo ""
            fi
        done
    else
        echo "qemu-aarch64 no está instalado y no es una arquitectura ARM de 64 bits. No se pueden ejecutar los archivos."
        exit 1
    fi
fi

exit 0