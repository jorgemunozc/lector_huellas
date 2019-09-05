# lector_huellas
Programa para escanear, enrolar y/o verificar huella digital

# Pre-requisitos
Instaladas librerías:
- Libfprint [https://fprint.freedesktop.org]
- MySQL C API [https://dev.mysql.com/downloads/connector/c/]

# Instalando librerías en Debian (Linux) y Raspbian (por confirmar)
Para instalar estas librerías en Debian utilizamos los siguientes comandos:
- Instalar libfprint
``` sudo apt install libfprint0 libfprint-dev``` 

- Instalar MySQL API Connector/C
```sudo apt install default-libmysqlclient-dev```

# Sobre archivos
Archivo principal para enrolar huella y guardarla en BD:  *enrolar.c*
- Para compilar este archivo solo basta ingresar `make enrolar` en la terminal.
- Para compilar el archivo que verifica las huellas: *verificar.c*

``` make verificar ```

Todos los archivos binarios generados quedan almacenados en la carpeta *bin*

## db_driver.h
Variables de entorno para configurar la conexion a la BD

## huella.h
Modulo que tendrá las funciones que se encarguen de manipular la huella almacenada en BD

# Problemas que pueden aparecer

## Descargar y compilar libfprint desde source code
### No se puede compilar el proyecto con meson
Para compilar correctamente la librería necesitamos las siguientes dependencias:
- libusb
- cmake
- meson
- python3 y pip3
- ninja
- libnss
Para instalar todas estas librerias solo basta correr la siguiente linea de comandos en la terminal
```sudo apt install libusb-1.0-0 libusb-1.0-0-dev pyhton3-pip python3-setuptools python3-wheel ninja-build && pip3 install meson```

Para instalar el driver de URU 4000 series:
``` sudo apt install libnss3-dev ```

