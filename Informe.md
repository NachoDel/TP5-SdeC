
# Universidad Nacional de Córdoba


## *Facultad de ciencias exactas, físicas y naturales*

### Sistemas de Computación – Practico \#5: Device Drivers

##### Alumnos: 
- Delamer, Ignacio
- Rivarola, Ignacio Agustin
- Verstraete, Enzo Gabriel

## **Consigna:**
Para superar este TP tendrán que diseñar y construir un CDD que permita sensar dos señales externas con un periodo de UN segundo. Luego una aplicación a nivel de usuario deberá leer UNA de las dos señales y graficarla en función del tiempo. La aplicación tambien debe poder indicarle al CDD cuál de las dos señales leer. Las correcciones de escalas de las mediciones, de ser necesario, se harán a nivel de usuario. Los gráficos de la señal deben indicar el tipo de señal que se está sensando, unidades en abcisas y tiempo en ordenadas. Cuando se cambie de señal el gráfico se debe "resetear" y acomodar a la nueva medición.


## **Objetivo General**
Diseñar, programar y probar un driver de caracter que permita sensar dos señales externas con un periodo de UN segundo.
* Diseñar y escribir el codigo de un programa para el ESP32 que mande 2 señales por uart mediante un USB hacia la computadora.
* Diseñar y escribir un driver CDD que obtenga las señales del puerto usb y los guarde en un buffer.  
* Diseñar y escribir un programa a nivel de usuario que nos permita visualizar una de las 2 señales elegidas.
  
## **Dearrollo**  

### **Programa ESP32**  
  
Primero se planteo un programa para el ESP32 que nos mande 2 señales diferentes con un periodo de 1 segundo por UART, por lo que se hizo el program en el cual se manda una señal pulsante con amplitud 10 y tambien una señal tipo dieente de sierra con una amplitud de 1 a 10.  
Uno de los puntos importantes es eel formato de los datos que se mandan esto debido a que el driver debe saber el formato de los datos a recibir, en este caso se manda la amplitud de la señales con el formato:  

        Amplitud señal 1 , Amplitud señal 2 = Y1,Y2/n
  
O sea se manda las amplitudes de las 2 señales, separadas por una coma y por linea, esto nos permite saber que va a recibir el driver, ¿porque es importante esto? nuestro driver debe saber que debe el formato de datos que recibe, independientemente de la señal que se mande.  
En la carpeta "Platformio/esp32_TP5" se encuentra el proyecto del programa del ESP que fue programado en C con el framework ESP-IDF usando PlatformIO. El codigo principal se encuentra en el archivo llamado main.c dentro de la carpeta src dentro del proyecto ya mencionado.  
En este codigo lo que se hace es configurar el UART0 que es el conectado al USB C, se forman las 2 señales y se se mandan por UART, como pueden ver, es muy simple.  
  
### **Driver de caracter(CDD)**

El driver de carácter desarrollado cumple una función central en la arquitectura del sistema: actúa como intermediario entre el dispositivo que genera las señales (ESP32 vía UART) y la aplicación en espacio de usuario. A continuación se explican las funciones más relevantes y su lógica de operación:

- Abrir el puerto serie (`/dev/ttyUSB0`) y lee los datos enviados por el ESP32.
 - Almacenar los datos leídos en un buffer protegido por mutex.
 - Exponer la información leída a través de un dispositivo de caracter en `/dev/signal_driver`.

Tambien realiza una **Lectura en espacio de usuario:** Al leer `/dev/signal_driver`, se obtiene una lista con líneas del tipo `Y1,Y2`.

**Fragmento relevante del Driver "FUNCIONES PRINCIPALES":**

**reader_function**

Esta función se ejecuta en un hilo del kernel creado específicamente para realizar la lectura continua del puerto serie (/dev/ttyUSB0).

ret = kernel_read(serial_filp, &ch, 1, &pos);
Lee carácter por carácter la información enviada por el ESP32, con una pausa controlada por msleep(READ_INTERVAL_MS). Los datos leídos (formato Y1,Y2\n) se almacenan en un buffer circular protegido con mutex para evitar condiciones de carrera.

**device_read**
Es la función que implementa la lectura del dispositivo desde espacio de usuario:

copy_to_user(user_buffer, buffer + *offset, len);

Transfiere al usuario los datos almacenados en el buffer. Se controla el puntero de offset para evitar lecturas fuera del rango actual del buffer.


**signal_init y signal_exit**

Se encargan de la inicialización y liberación del módulo siguiendo los siguiente pasos:

  Se registra un dispositivo de carácter dinámico.

  Se crea una clase y un dispositivo visible en /dev/signal_driver.

  Se lanza el hilo reader_thread para comenzar a leer el puerto serie.

**SINCRONIZACION** 

  Se utiliza como:
  
  DEFINE_MUTEX(buffer_mutex);

Este mutex garantiza que el buffer compartido por el hilo lector y las llamadas read del usuario no se corrompa al ser accedido simultáneamente.


### **Programa nivel de usuario**

El programa a nivel de usuario se encarga de

- Leer datos del driver (`/dev/signal_driver`), separando las listas de valores de Y1 y Y2.
- Permitir al usuario seleccionar cuál de las dos señales graficar (Y1 o Y2).
- Al cambiar de señal, el gráfico se resetea y se ajusta a la nueva medición.
- El eje X representa el tiempo (segundos), el eje Y la amplitud; el gráfico indica el tipo de señal sensada.
    
Como la consigna lo solicitaba, el programa a nivel de usuario permite la **selección de señal** permitiendo al usuario elegir por teclado cuál visualizar y el gráfico se actualiza en consecuencia.

**Fragmento relevante del programa a nivel de usuario:**
```python
while True:
    opcion = input("¿Qué señal deseas graficar? (1 para Y1, 2 para Y2, 'q' para salir): ")
    graficar(opcion, x_vals, y1_vals, y2_vals)
```
**PRUEBAS REALIZADAS**


Para verificar el funcionamiento se hizo una prueba completa de lectura, selección y graficado. En la prueba se realizaron las siguientes acciones:

  1)Inicialización del ESP32, enviando dos señales distintas: una pulsante (amplitud fija de 10) y una diente de sierra (1 a 10).

  2)Carga del módulo CDD, creando el dispositivo /dev/signal_driver.

  3)Ejecución del programa de usuario, con lectura y graficado de señales.

  4)Cambio de señal durante la ejecución, verificando que el gráfico se reinicia correctamente y representa la señal deseada.

A continuación se insertarán imágenes ilustrando los gráficos de ambas señales:



## **Conclusiones**
En conclusion, la arquitectura planteada y el código implementado cumplen integralmente la consigna: permiten sensar dos señales externas (simuladas por un ESP32), exponerlas mediante un driver de caracter y visualizarlas en una aplicación de usuario que permite seleccionar cuál graficar, reiniciando la visualización al cambiar de señal.
  
  




