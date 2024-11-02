#!/bin/bash
# Verificar si se proporcionaron al menos un parámetro
if [ $# -lt 1 ]; then
    echo "Uso: $0 <tamanho N> [<seed>]"
    exit 1
fi

tamanhoN=$1
seed=$2

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

# Ejecutar todos los archivos sin extensión en el directorio actual una vez, ignorando .sh
for file in *; do
    if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
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



# Ejecutar todos los archivos con extensión .o en el directorio actual
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


# Ejecutar todos los archivos con extensión .out en el directorio actual
for file in *.out; do
    if [ -f "$file" ] && [ -x "$file" ]; then
        if [ -z "$seed" ]; then
            echo "Ejecutando $file con N $tamanhoN"
            qemu-aarch64 ./"$file" "$tamanhoN"
        else
            echo "Ejecutando $file con N $tamanhoN y seed $seed"
            qemu-aarch64 ./"$file" "$tamanhoN" "$seed"
        fi
    fi
done

echo ""
