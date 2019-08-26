# lector_huellas
Programa para escanear, enrolar y/o verificar huella digital

# Pre-requisitos
Instaladas librerías:
- Libfprint [https://fprint.freedesktop.org]
- MySQL C API [https://dev.mysql.com/downloads/connector/c/]

# Instalando librerías en Debian (Linux)
Para instalar estas librerías en Debian utilizamos los siguientes comandos:
- Instalar libfprint
``` sudo apt install libfprint0 libfprint-dev``` 

- Instalar MySQL API Connector/C
```sudo apt install default-libmysqlclient-dev```

# Sobre archivos
Archivo principal para enrolar huella y guardarla en BD:  *enrolar.c*
Para compilar este archivo solo basta ingresar `make enrolar` en la terminal. El
archivo binario se crea en la carpeta `bin`
