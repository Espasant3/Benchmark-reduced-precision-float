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
source entorno_TFG/bin/activate

# Instalar los paquetes
pip install -r requirements.txt

# Salir del entorno virtual (opcional)
deactivate
