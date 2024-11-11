# Relación Señal-Ruido de Pico

import numpy as np
from PIL import Image

def calcular_psnr(imagen_original, imagen_comprimida):
    # Convertir las imágenes a arrays de numpy
    imagen_original = np.array(imagen_original)
    imagen_comprimida = np.array(imagen_comprimida)

    # Calcular la diferencia entre las imágenes
    diferencia = imagen_original - imagen_comprimida

    # Calcular la potencia del ruido (MSE)
    mse = np.mean(diferencia ** 2)

    # Calcular la potencia máxima de la señal (MAX)
    max_valor = 255

    # Calcular la PSNR
    psnr = 10 * np.log10((max_valor ** 2) / mse)

    return psnr

# Cargar las imágenes
imagen_original = Image.open('imagen_original.jpg')
imagen_comprimida = Image.open('imagen_comprimida.jpg')

# Calcular la PSNR
psnr = calcular_psnr(imagen_original, imagen_comprimida)

print('La PSNR es:', psnr)
