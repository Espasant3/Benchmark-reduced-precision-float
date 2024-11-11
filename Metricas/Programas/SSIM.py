# Coeficiente de Correlacion Estructural

import numpy as np
from PIL import Image
import argparse
from skimage.measure import compare_ssim

def calcular_ssim(imagen_original, imagen_comprimida):
    # Convertir las imágenes a arrays de numpy
    imagen_original = np.array(imagen_original)
    imagen_comprimida = np.array(imagen_comprimida)

    # Calcular el SSIM
    ssim = compare_ssim(imagen_original, imagen_comprimida, multichannel=True)

    return ssim

def main():
    parser = argparse.ArgumentParser(description='Calcular el Coeficiente de Correlación Estructural (SSIM)')
    parser.add_argument('imagen_original', help='Imagen original')
    parser.add_argument('imagen_comprimida', help='Imagen comprimida')

    args = parser.parse_args()

    imagen_original = Image.open(args.imagen_original)
    imagen_comprimida = Image.open(args.imagen_comprimida)

    # Asegurarse de que las imágenes tengan el mismo tamaño
    if imagen_original.size != imagen_comprimida.size:
        print("Las imágenes deben tener el mismo tamaño")
        sys.exit(1)

    ssim = calcular_ssim(imagen_original, imagen_comprimida)

    print("El Coeficiente de Correlación Estructural (SSIM) es:", ssim)

if __name__ == "__main__":
    main()
