# **SISTEMAS DE COMPUTACION**
# **TRABAJO PRACTICO N° 5: DRIVER DE CARACTER**

## INTEGRANTES:
- Delamer, Ignacio
- Rivarola, Ignacio
- Verstraete, Enzo

## GRUPO: 
CODE GENESIS

## INDICE:  
Carpeta 'CDD' --> Codigo del driver y mekefile para se compilacion.  
Carpeta 'img' --> Imagenes presentes en el informe.  
Carpeta 'Platformio' --> Proyecto del ESP32.   
Carpeta 'src' --> Codigos del programa a nivel de uduario que grafica las señales.   
Markdowm 'README' --> Instructivo para la ejecucion.  
Markdown 'Informe.md' --> Informe completo de todo el trabajo.  


## INSTRUCTIVO DE USUARIO: 
  
Testeo completo del proyecto de driver y visualización

1. Preparar el ESP32
  
    Asegurate de que el ESP32 esté conectado por USB a tu PC.  
    Verificá que esté corriendo el programa que envía datos por UART con el formato 123,456\n cada 0.1 segundos.  
  
2. Verificar que el ESP32 aparezca como dispositivo USB
    Listá los puertos USB:
  
        ls /dev/ttyUSB*  
  
    Deberías ver algo como:
  
        /dev/ttyUSB0  
  
3. Compilar y cargar el módulo del kernel (CDD)
     
    a) Compilar:

        make  
  
    b) Cargar el módulo:

        sudo insmod signal_driver.ko  
  
    c) Verificar la carga del modulo:

       lsmod | grep signal_driver 
  
    d) Crear el dispositivo en /dev/ y cambia los permisos de lectura:  
       Comprobar major number:
   
           cat /proc/devices | grep signal_driver
           sudo mknod /dev/signal_driver c <major> 0
  
       Por ejemplo, si viste 'major 240':

           sudo mknod /dev/signal_driver c 240 0  
  

      Cambio de permisos:

           sudo chmod 666 /dev/signal_driver 
  
4. Verificar lectura manual
     
    Leé desde el driver con cat:

        cat /dev/signal_driver  
  
    Deberías ver un valor una lista con valores tipo: y1,y2.  
  
5. Ejecutar el graficador en Python  

        python3 signal_plotter.py  

