#!/bin/bash

# Array para almacenar los archivos con errores de compilación
errores=()

# Compilar todos los archivos .c del directorio actual con gcc-14 y opciones -Wall y -g
for file in *.c
do
    # Obtener el nombre del archivo sin la extensión .c
    base_name=$(basename "$file" .c)
    
    # Compilar el archivo con las opciones necesarias para Valgrind
    if ! gcc-14 -Wall -g "$file" -o "$base_name"
    then
        # Si hay un error, añadir el archivo al array de errores
        errores+=("$file")
    else
        echo "Compilado $file a $base_name"
    fi
done

# Verificar si hubo errores
if [ ${#errores[@]} -ne 0 ]
then
    echo "Errores de compilación en los siguientes archivos:"
    for error_file in "${errores[@]}"
    do
        echo "$error_file"
    done
else
    echo "Todos los archivos compilados correctamente."
fi
