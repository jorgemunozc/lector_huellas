CC = gcc
CFLAGS = -g 
FPRINTFLAGS = -L="/usr/include/libfprint" -lfprint
MYSQLFLAGS = `mysql_config --cflags --libs`

test:
	${CC} ${CFLAGS} ${FPRINTFLAGS} main.c -o bin/test

verificar:
	${CC} ${CFLAGS} ${MYSQLFLAGS} verificar.c db_config.c -o bin/verificar

enrolar:
	${CC} ${CFLAGS} ${FPRINTFLAGS} ${MYSQLFLAGS} enrolar.c db_config.c huella.c -o bin/enrolar

all:
	${CC} ${CFLAGS} ${FPRINTFLAGS} ${MYSQLFLAGS} test.c db_config.c -o bin/verifyprint

ejemplo:
	${CC} ${CFLAGS} ${FPRINTFLAGS} verify_live.c -o bin/examples/verify_live