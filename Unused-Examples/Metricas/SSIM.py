# Índice de Similitud Estructural (SSIM)
import numpy as np
import argparse
import csv
from PIL import Image
import sys
from skimage.metrics import structural_similarity as ssim
from collections import defaultdict
from pathlib import Path


def cargar_datos_csv(ruta_archivo):
    """Carga datos desde CSV y reconstruye matrices por ejecución."""
    datos_por_ejecucion = defaultdict(list)
    
    with open(ruta_archivo, 'r') as f:
        reader = csv.DictReader(f)
        for fila in reader:
            ejecucion = int(fila['Ejecucion'])
            fila_idx = int(fila['Fila'])
            columna_idx = int(fila['Columna'])
            valor = float(fila['Valor'])
            datos_por_ejecucion[ejecucion].append((fila_idx, columna_idx, valor))
    
    # Reconstruir matrices por ejecución
    matrices = []
    for ejecucion in sorted(datos_por_ejecucion.keys()):
        # Determinar dimensiones máximas
        filas = max(d[0] for d in datos_por_ejecucion[ejecucion]) + 1
        columnas = max(d[1] for d in datos_por_ejecucion[ejecucion]) + 1
        matriz = np.zeros((filas, columnas))
        for f, c, v in datos_por_ejecucion[ejecucion]:
            matriz[f, c] = v
        matrices.append(matriz)
    
    return matrices

def calcular_ssim(original, comprimida):
    """Calcula SSIM entre dos matrices/arrays."""
    if original.shape != comprimida.shape:
        raise ValueError("Las dimensiones de los datos no coinciden")
    return ssim(original, comprimida, data_range=max(original.max(), comprimida.max()))

def guardar_resultado_csv(nombre_archivo, resultados, modo):
    """Guarda los resultados SSIM en un CSV, omitiendo 'Ejecución' si solo hay una."""
    nombre_completo = f"{nombre_archivo}.csv"

    # Determinar si hay solo una ejecución
    single_execution = all(ejec == 1 for ejec, _ in resultados)

    with open(nombre_completo, 'w', newline='') as f:
        # Escribir metadatos como comentarios
        f.write(f"# Resultados {modo}\n")

        # Escribir encabezado según el número de ejecuciones
        if single_execution:
            f.write("Configuración,SSIM\n")
        else:
            f.write("Configuración,Ejecución,SSIM\n")

        # Escribir los resultados
        for ejec, score in resultados:
            if single_execution:
                f.write(f"{Path(nombre_archivo).stem},{score:.8f}\n")
            else:
                f.write(f"{Path(nombre_archivo).stem},{ejec},{score:.8f}\n")

    print(f"Resultados guardados en '{nombre_completo}'")

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

    try:
        if args.csv:
            original_matrices = cargar_datos_csv(args.source)
            compressed_matrices = cargar_datos_csv(args.compressed)
            modo = "CSV"
        elif args.image:
            original_matrices = [np.array(Image.open(args.source))]
            compressed_matrices = [np.array(Image.open(args.compressed))]
            modo = "Imagen"

        # Validar número de ejecuciones
        if len(original_matrices) != len(compressed_matrices):
            raise ValueError("Número de ejecuciones no coincide")

        # Calcular SSIM por ejecución
        resultados = []
        for i, (orig, comp) in enumerate(zip(original_matrices, compressed_matrices), 1):
            if orig.shape != comp.shape:
                raise ValueError(f"Ejecución {i}: Dimensiones no coinciden")
            ssim_score = calcular_ssim(orig, comp)
            resultados.append((i, ssim_score))

        print("Resultados SSIM:")
        for idx, score in resultados:
            print(f"Ejecución {idx}: SSIM = {score:.6f}")

        # Guardar resultados si se especificó -o
        if args.output:
            guardar_resultado_csv(args.output, resultados, modo)

    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Error inesperado: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()