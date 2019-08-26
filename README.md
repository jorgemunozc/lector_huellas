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