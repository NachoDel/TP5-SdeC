import matplotlib.pyplot as plt
import numpy as np

# Función para leer los datos
def leer_datos():
    y1_vals, y2_vals = [], []
    with open("/dev/signal_driver", "r") as f:
        for linea in f:
            try:
                y1, y2 = map(int, linea.strip().split(","))  # Confirmamos que los datos son enteros
                y1_vals.append(y1)
                y2_vals.append(y2)
            except ValueError:
                pass  # Ignorar líneas mal formateadas
    return y1_vals, y2_vals

# Función para graficar la señal seleccionada
def graficar(opcion, x_vals, y1_vals, y2_vals):
    plt.figure(figsize=(8, 5))
    if opcion == "1":
        plt.plot(x_vals, y1_vals, marker="o", linestyle="-", label="Serie Y1", color="blue")
    elif opcion == "2":
        plt.plot(x_vals, y2_vals, marker="s", linestyle="-", label="Serie Y2", color="red")
    
    plt.xlabel("Tiempo (s)")
    plt.ylabel("Amplitud")
    plt.title(f"Gráfica de {'Y1' if opcion == '1' else 'Y2'}")
    plt.legend()
    plt.grid()

    plt.xticks(np.arange(0, 2.1, 0.1))  # Divisiones cada 0.1s
    plt.xlim(0, 2)  # Limitar el eje X a 2 segundos
    plt.show()

# Leer los datos iniciales
y1_vals, y2_vals = leer_datos()

# Encontrar el índice donde (y1, y2) == (0,1) para alinear el eje X
try:
    index_zero = next(i for i, (y1, y2) in enumerate(zip(y1_vals, y2_vals)) if y1 == 0 and y2 == 1)
except StopIteration:
    index_zero = 0  # Si no se encuentra, usar el primer valor

# Construir el eje X en segundos desde 0 hasta 2 s con divisiones de 0.1 s
x_vals = np.arange(0, len(y1_vals) * 0.1, 0.1)  
x_vals -= x_vals[index_zero]  # Ajustar el eje X para que el 0 coincida con el punto (0,1)

# Interacción con el usuario
while True:
    opcion = input("\n¿Qué señal deseas graficar? (1 para Y1, 2 para Y2, 'q' para salir): ").strip()
    
    if opcion == "q":
        print("Saliendo del programa...")
        break  # Salir del bucle
    
    if opcion not in ["1", "2"]:
        print("Entrada inválida. Ingresa 1 para Y1, 2 para Y2 o 'q' para salir.")
        continue
    
    graficar(opcion, x_vals, y1_vals, y2_vals)


