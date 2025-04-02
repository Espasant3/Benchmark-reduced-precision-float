# Coeficiente de Correlación Estructural (SSIM)
import numpy as np
import argparse
import csv
from PIL import Image
import sys
from skimage.metrics import structural_similarity as ssim

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

def calcular_ssim(original, comprimida):
    """Calcula SSIM entre dos matrices/arrays."""
    if original.shape != comprimida.shape:
        raise ValueError("Las dimensiones de los datos no coinciden")
    return ssim(original, comprimida, data_range=max(original.max(), comprimida.max()))

def guardar_resultado_csv(nombre_archivo, ssim_score, modo):
    """Guarda el resultado SSIM en un CSV."""
    nombre_completo = f"{nombre_archivo}.csv"
    with open(nombre_completo, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Modo", "SSIM"])
        writer.writerow([modo, f"{ssim_score:.6f}"])
    print(f"Resultado guardado en '{nombre_completo}'")

def parse_arguments():
    parser = argparse.ArgumentParser(description='Calcular SSIM')
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
    try:
        if args.csv:
            original = cargar_datos_csv(args.source)
            compressed = cargar_datos_csv(args.compressed)
            modo = "CSV"
        elif args.image:
            original = np.array(Image.open(args.source))
            compressed = np.array(Image.open(args.compressed))
            modo = "Imagen"

        # Cálculo de SSIM
        ssim_score = calcular_ssim(original, compressed)
        print(f'SSIM: {ssim_score:.6f}')

        # Guardar resultado si se especificó -o
        if args.output:
            guardar_resultado_csv(args.output, ssim_score, modo)

    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Error inesperado: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()