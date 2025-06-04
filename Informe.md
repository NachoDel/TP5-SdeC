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



### **Programa nivel de usuario**



## **Conclusiones**
  
  
  
  




