import matplotlib.pyplot as plt
import numpy as np

# Listas para almacenar datos
y1_vals = []
y2_vals = []

# Leer datos desde /dev/signal_driver
with open("/dev/signal_driver", "r") as f:
    for linea in f:
        try:
            y1, y2 = map(int, linea.strip().split(","))  # Cambiado a int en lugar de float
            y1_vals.append(y1)
            y2_vals.append(y2)
        except ValueError:
            pass  # Ignorar líneas mal formateadas

print("Total de muestras de Y1: ", len(y1_vals), " de Y2: ", len(y2_vals))
print("Y1:", y1_vals)
print("Y2:", y2_vals)

# Encontrar el índice donde (y1, y2) == (0,1)
try:
    index_zero = next(i for i, (y1, y2) in enumerate(zip(y1_vals, y2_vals)) if y1 == 0 and y2 == 1)
except StopIteration:
    index_zero = 0  # Si no se encuentra, usar el primer valor

# Construir eje X en segundos desde 0 hasta 2 s con pasos de 0.1 s
x_vals = np.arange(0, len(y1_vals) * 0.1, 0.1)  # Suponiendo que cada muestra es cada 0.1s
x_vals -= x_vals[index_zero]  # Ajustar el eje X para que 0 corresponda al índice encontrado

# Graficar
plt.figure(figsize=(8, 5))
plt.plot(x_vals, y1_vals, marker="o", linestyle="-", label="Serie Y1", color="blue")
plt.plot(x_vals, y2_vals, marker="s", linestyle="-", label="Serie Y2", color="red")
plt.xlabel("Tiempo (s)")
plt.ylabel("Valores (enteros)")
plt.title("Gráfica ajustada de Y1 y Y2 con datos enteros")
plt.legend()
plt.grid()

# Ajustar escala del eje X
plt.xticks(np.arange(0, 2.1, 0.1))  # Marcas cada 0.1s de 0 a 2s
plt.xlim(0, 2)  # Limitar el eje X a 2 segundos

plt.show()

