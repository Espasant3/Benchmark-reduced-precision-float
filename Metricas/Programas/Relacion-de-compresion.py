import numpy as np
from PIL import Image
import argparse

def calcular_cr_señal(señal_original, señal_comprimida):
    # Calcular la longitud de la señal original
    longitud_original = len(señal_original)

    # Calcular la longitud de la señal comprimida
    longitud_comprimida = len(señal_comprimida)

    # Calcular la relación de compresión
    cr = longitud_original / longitud_comprimida

    return cr

def calcular_cr_imagen(imagen_original, imagen_comprimida):
    # Convertir las imágenes a arrays de numpy
    imagen_original = np.array(imagen_original)
    imagen_comprimida = np.array(imagen_comprimida)

    # Calcular el tamaño de la imagen original
    tamaño_original = imagen_original.size

    # Calcular el tamaño de la imagen comprimida
    tamaño_comprimida = imagen_comprimida.size

    # Calcular la relación de compresión
    cr = tamaño_original / tamaño_comprimida

    return cr

def main():
    parser = argparse.ArgumentParser(description='Calcular la relación de compresión')
    parser.add_argument('dimension', choices=['1d', '2d'], help='Dimensión de los datos')
    parser.add_argument('archivo_original', help='Archivo original')
    parser.add_argument('archivo_comprimido', help='Archivo comprimido')

    args = parser.parse_args()

    if args.dimension == '1d':
        señal_original = np.loadtxt(args.archivo_original)
        señal_comprimida = np.loadtxt(args.archivo_comprimido)
        cr = calcular_cr_señal(señal_original, señal_comprimida)
    elif args.dimension == '2d':
        imagen_original = Image.open(args.archivo_original)
        imagen_comprimida = Image.open(args.archivo_comprimido)
        cr = calcular_cr_imagen(imagen_original, imagen_comprimida)

    print("La relación de compresión es:", cr)

if __name__ == "__main__":
    main()
