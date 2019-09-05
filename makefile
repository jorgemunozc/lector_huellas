CC = gcc
CFLAGS = -g -I /usr/include/libusb-1.0 `pkg-config --cflags glib-2.0` `pkg-config --libs glib-2.0` -I libs/libfprint
FPRINTFLAGS = -L="/usr/include/libfprint" -lfprint -L="libs/libfprint"
# FPRINTFLAGSLOCAL = -I "./libs/libfprint/libfprint"
MYSQLFLAGS = `mysql_config --cflags --libs`

test:
	${CC} ${CFLAGS} ${FPRINTFLAGS} main.c -o bin/test

verificar:
	${CC} ${CFLAGS} ${MYSQLFLAGS} verificar.c db_driver.h -o bin/verificar

enrolar:
	${CC} ${CFLAGS} ${MYSQLFLAGS} enrolar.c db_driver.c huella.c verificar.c -o bin/enrolar

all:
	${CC} ${CFLAGS} ${FPRINTFLAGS} ${MYSQLFLAGS} test.c db_config.c -o bin/verifyprint

ejemplo:
	${CC} ${CFLAGS} ${FPRINTFLAGS} verify_live.c -o bin/examples/verify_live