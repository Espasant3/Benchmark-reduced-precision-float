# Use una imagen base de Python 3.12 con soporte para gcc (para compilar los programas en C)
FROM python:3.12-slim

# Instale gcc y otras herramientas necesarias
RUN apt-get update && apt-get install -y \
    gcc-14 \
    make \
    aarch64-linux-gnu-gcc \
    qemu-user \
    gcc-arm-none-eabi \
    gcc-arm-linux-gnueabihf \
    && rm -rf /var/lib/apt/lists/*

# Cree los directorios necesarios
RUN mkdir -p /Programas/AXPY /Programas/DCT /Programas/PCA /Programas/DWT_1D /Tests-Python /Metricas

# Copie los programas C y Python al contenedor
COPY /Programas/ /Programas/
COPY /Tests-Python/ /Tests-Python/
COPY /Metricas/ /Metricas/

# Establezca el directorio de trabajo
WORKDIR /Programas/AXPY

# Copie los scripts de compilación y ejecución
COPY /scripts/compile_axpy.sh /Programas/AXPY/
COPY /scripts/run_axpy.sh /Programas/AXPY/
RUN chmod +x /Programas/AXPY/compile_axpy.sh /Programas/AXPY/run_axpy.sh

COPY /scripts/compile_dct.sh /Programas/DCT/
COPY /scripts/run_dct.sh /Programas/DCT/
RUN chmod +x /Programas/DCT/compile_dct.sh /Programas/DCT/run_dct.sh

# Compile los programas en C usando el script de compilación
RUN ./compile_axpy.sh
RUN ./compile_dct.sh

# Monte el entorno virtual de Python y instale las dependencias
WORKDIR /Tests-Python
RUN python3 -m venv entorno_TFG && \
    ./entorno_TFG/bin/pip install --upgrade pip && \
    ./entorno_TFG/bin/pip install -r requirements.txt

# Copie el script de entorno virtual y hágalo ejecutable
COPY ./setup_env.sh /Tests-Python/
RUN chmod +x setup_env.sh

# Comando por defecto para activar el entorno y ejecutar los scripts necesarios
CMD ["bash", "-c", "source ./setup-python-env.sh && python3 run_tests.py"]
