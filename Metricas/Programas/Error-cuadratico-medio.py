import numpy as np

def mean_squared_error(y_true, y_pred):
    """
    Calcula el Error Cuadrático Medio entre dos vectores.

    :param y_true: Vector de valores reales.
    :param y_pred: Vector de valores predichos.
    :return: Error Cuadrático Medio.
    """
    mse = np.mean((y_true - y_pred) ** 2)
    return mse

# Ejemplo de uso
y_true = np.array([1.0, 2.0, 3.0, 4.0, 5.0], dtype=float)
y_pred = np.array([1.2, 1.9, 2.8, 4.1, 4.9], dtype=float)

mse = mean_squared_error(y_true, y_pred)
print(f"El Error Cuadrático Medio es: {mse:.6f}")
