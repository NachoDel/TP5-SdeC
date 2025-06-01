✅ Testeo completo del proyecto de driver y visualización

🟢 1. Preparar el ESP32

Asegurate de que el ESP32 esté conectado por USB a tu PC.

Verificá que esté corriendo el programa que envía datos por UART con el formato 123,456\n cada 0.1 segundos.

🟢 2. Verificar que el ESP32 aparezca como dispositivo USB
Listá los puertos USB:

bash
Copiar
Editar
ls /dev/ttyUSB*
Deberías ver algo como:

bash
Copiar
Editar
/dev/ttyUSB0
🟢 3. Compilar y cargar el módulo del kernel (CDD)
a) Compilar:
bash
Copiar
Editar
make
b) Cargar el módulo:
bash
Copiar
Editar
sudo insmod signal_driver.ko
c) Verificar en el log del kernel:
bash
Copiar
Editar
dmesg | tail
Deberías ver:

lua
Copiar
Editar
signal_driver loaded: major <num>
d) Crear el dispositivo en /dev/ (usando el número mayor que viste):
bash
Copiar
Editar
sudo mknod /dev/signal_driver c <major> 0
sudo chmod 666 /dev/signal_driver
Por ejemplo, si viste major 240:

bash
Copiar
Editar
sudo mknod /dev/signal_driver c 240 0
🟢 4. Verificar lectura manual
Leé desde el driver con cat:

bash
Copiar
Editar
cat /dev/signal_driver
Deberías ver un valor (ejemplo: 123), que cambia cada 1 segundo.

🟢 5. Cambiar de señal (opcional)
Escribí un 0 o 1 en el driver:

bash
Copiar
Editar
echo 1 > /dev/signal_driver
cat /dev/signal_driver
🟢 6. Ejecutar el graficador en Python
bash
Copiar
Editar
python3 signal_plotter.py
Ingresá 0 o 1 al iniciar.

Presioná tecla 0 o 1 mientras corre para cambiar de señal.

Verificá que el gráfico se actualice cada 1 segundo.

Verificá que se resetee al cambiar de señal.