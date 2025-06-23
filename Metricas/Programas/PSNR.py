# Proporción Máxima de Señal a Ruido o Peak Signal-to-Noise Ratio (PSNR)
import numpy as np
import argparse
import csv
from PIL import Image
import os

def cargar_datos_csv(ruta_archivo):
    """Carga datos desde CSV y reconstruye matriz."""
    datos = []
    with open(ruta_archivo, 'r') as f:
        reader = csv.DictReader(f)
        for fila in reader:
            datos.append([float(fila['Fila']), float(fila['Columna']), float(fila['Valor'])])
    filas = int(max(d[0] for d in datos)) + 1
    cols = int(max(d[1] for d in datos)) + 1
    matriz = np.zeros((filas, cols))
    for d in datos:
        matriz[int(d[0]), int(d[1])] = d[2]
    return matriz

def calcular_psnr(original, comprimida):
    """Calcula PSNR entre dos matrices/arrays."""
    diferencia = original - comprimida
    mse = np.mean(diferencia ** 2)
    max_valor = 255 if original.dtype == np.uint8 else np.max(original)
    psnr = 10 * np.log10((max_valor ** 2) / mse) if mse != 0 else float('inf')
    return psnr

def guardar_resultado_csv(nombre_archivo, psnr, modo):
    """Guarda el resultado PSNR en un CSV."""
    nombre_completo = f"{nombre_archivo}.csv"
    with open(nombre_completo, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Modo", "PSNR (dB)"])
        writer.writerow([modo, f"{psnr:.5f}"])
    print(f"Resultado guardado en '{nombre_completo}'")

def parse_arguments():
    parser = argparse.ArgumentParser(description='Calcular PSNR')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--csv', action='store_true', help='Usar archivos CSV')
    group.add_argument('--image', action='store_true', help='Usar imágenes (JPEG/PNG)')
    parser.add_argument('-s', '--source', required=True, help='Archivo original (source)')
    parser.add_argument('-c', '--compressed', required=True, help='Archivo comprimido')
    parser.add_argument('-o', '--output', help='Nombre del archivo de salida (sin extensión)')
    return parser.parse_args()

def main():
    args = parse_arguments()

    # Carga de datos
    if args.csv:
        original = cargar_datos_csv(args.source)
        compressed = cargar_datos_csv(args.compressed)
        modo = "CSV"
    elif args.image:
        original = np.array(Image.open(args.source))
        compressed = np.array(Image.open(args.compressed))
        modo = "Imagen"

    # Cálculo de PSNR
    psnr = calcular_psnr(original, compressed)
    print(f'PSNR: {psnr:.5f} dB')

    # Guardar resultado si se especificó el flag -o
    if args.output:
        guardar_resultado_csv(args.output, psnr, modo)

if __name__ == "__main__":
    main()