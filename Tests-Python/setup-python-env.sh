#!/bin/bash

# Cambiar al directorio donde se encuentra el script
cd "$(dirname "$0")"

# Comprobar si el entorno virtual ya existe
if [ ! -d "entorno_TFG" ]; then
    # Crear el entorno virtual si no existe
    python3 -m venv entorno_TFG
    echo "Entorno virtual creado."
else
    echo "El entorno virtual ya existe."
fi

# Activar el entorno virtual
source "$(pwd)/entorno_TFG/bin/activate"

# Instalar los paquetes sin mostrar la salida estándar
pip install -r requirements.txt > /dev/null 2>&1

deactivate