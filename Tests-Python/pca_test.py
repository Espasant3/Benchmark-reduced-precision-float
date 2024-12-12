import numpy as np
from sklearn.decomposition import PCA

def create_matrix(rows, cols, data):
    matrix = np.array(data).reshape(rows, cols)
    return matrix

def standardize_matrix(matrix):
    means = np.mean(matrix, axis=0)
    std_devs = np.std(matrix, axis=0)
    standardized_matrix = (matrix - means) / std_devs
    return standardized_matrix

def do_pca(matrix, n_components=None):
    standardized_matrix = standardize_matrix(matrix)
    pca = PCA(n_components=n_components)
    transformed_data = pca.fit_transform(standardized_matrix)
    return transformed_data, pca.explained_variance_ratio_

def print_matrix(matrix):
    for row in matrix:
        print("\t".join(f"{val:.8f}" for val in row))

if __name__ == "__main__":
    data = [
        [1.2, 0.8, 6.0, 5.7],
        [4.1, 1.5, 3.1, 1.1],
        [2.9, 3.3, 0.12, 4.4],
        [3.7, 2.1, 1.2, 7.3]
    ]
    rows, cols = 4, 4
    matrix = create_matrix(rows, cols, data)
    
    transformed_data, explained_variance_ratio = do_pca(matrix)
    
    print("Datos transformados (PCA aplicada):")
    print_matrix(transformed_data)
    
    print("\nVarianza explicada por cada componente principal:")
    print("\t".join(f"{val:.8f}" for val in explained_variance_ratio))