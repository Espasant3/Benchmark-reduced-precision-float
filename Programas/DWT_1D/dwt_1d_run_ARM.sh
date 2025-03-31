#!/bin/bash

# Función para comprobar si qemu-aarch64 está instalado
check_sde() {
    if command -v sde >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}
# Función para construir el mensaje (mejor legibilidad)
build_message() {
    local msg="Ejecutando $1 con N=$2"
    [ -n "$3" ] && msg+=" y seed=$3"       # Añade seed si existe
    [ -n "$verbose_flag" ] && msg+=" [verbose]"  # Añade verbose si está activo
    echo "$msg"
}

# Inicializar variables
force_run=false
verbose_flag=""
tamanhoN=""
seed=""

# Uso: $0 [-f|--force] <tamanho N> [<seed>]
usage() {
    echo "Uso: $0 [-f|--force] <tamanho N> [<seed>]"
    exit 1
}

# Procesar argumentos con GNU getopt
TEMP=$(getopt -o fvh --long force,help -n "$0" -- "$@")

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
        -v)
            verbose_flag="-v"
            shift
            ;;
        -h|--help)
            # Mostrar ayuda
            echo "Uso: $0 [-f|--force] [-v] <tamanho N> [<seed>]"
            echo "  -f, --force       Fuerza la compilación cruzada de todos los programas."
            echo "  -v                Muestra información adicional durante la ejecución."
            echo "  -h, --help        Muestra esta ayuda y sale."
            exit 0
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

echo "Ejecutando nativamente en arquitectura ARM de 64 bits"
for file in *.out; do
    if [ -f "$file" ] && [ -x "$file" ]; then
        echo "$(build_message "$file" "$tamanhoN" "$seed")"
        ./"$file" "$tamanhoN" "$seed" "$verbose_flag"
        echo ""
    fi
done


if $force_run; then
    if check_sde; then
        # Ejecutar todos los archivos sin extensión en el directorio actual una vez, ignorando .sh
        for file in *; do
            if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
                echo "$(build_message "$file" "$tamanhoN" "$seed")"
                sde -spr -- ./"$file" "$tamanhoN" "$seed" "$verbose_flag"  
                echo ""
            fi
        done
    else
        echo "sde no está instalado o no se encuentra en el PATH. No se puede ejecutar ningún archivo."
    fi
fi

exit 0
