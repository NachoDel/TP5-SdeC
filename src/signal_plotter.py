import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time

DEVICE_PATH = "/dev/signal_driver"

def select_signal(signal_number):
    with open(DEVICE_PATH, "w") as f:
        f.write(str(signal_number))

def read_signal():
    with open(DEVICE_PATH, "r") as f:
        return int(f.read().strip())

# Configuración inicial
signal_number = int(input("Elegí señal a graficar (0 o 1): "))
select_signal(signal_number)

x_data = []
y_data = []

fig, ax = plt.subplots()
line, = ax.plot([], [], label=f"Señal {signal_number}")
ax.set_xlabel("Tiempo (s)")
ax.set_ylabel("Valor")
ax.set_title(f"Graficando señal {signal_number}")
ax.legend()

start_time = time.time()

def update(frame):
    global signal_number, start_time, x_data, y_data

    try:
        value = read_signal()
    except Exception as e:
        print("Error leyendo la señal:", e)
        return line,

    t = round(time.time() - start_time, 1)
    x_data.append(t)
    y_data.append(value)

    if len(x_data) > 300:
        x_data = x_data[1:]
        y_data = y_data[1:]

    line.set_data(x_data, y_data)
    ax.relim()
    ax.autoscale_view()

    return line,

def on_key(event):
    global signal_number, x_data, y_data, start_time

    if event.key == "0" or event.key == "1":
        signal_number = int(event.key)
        select_signal(signal_number)
        x_data.clear()
        y_data.clear()
        start_time = time.time()
        line.set_label(f"Señal {signal_number}")
        ax.set_title(f"Graficando señal {signal_number}")
        ax.legend()
        print(f"Cambiado a señal {signal_number}")

ani = animation.FuncAnimation(fig, update, interval=200)
fig.canvas.mpl_connect("key_press_event", on_key)

plt.show()
