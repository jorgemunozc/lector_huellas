CC = gcc
CFLAGS = -g 
FPRINTFLAGS = -L="/usr/include/libfprint" -lfprint
MYSQLFLAGS = `mysql_config --cflags --libs`

test:
	${CC} ${CFLAGS} ${FPRINTFLAGS} main.c -o bin/test

connectdb:
	${CC} ${CFLAGS} ${MYSQLFLAGS} connectdb.c db_config.c -o bin/testDB

enrolar:
	${CC} ${CFLAGS} ${FPRINTFLAGS} ${MYSQLFLAGS} enrolar.c db_config.c huella.c -o bin/enrolar

all:
	${CC} ${CFLAGS} ${FPRINTFLAGS} ${MYSQLFLAGS} test.c db_config.c -o bin/verifyprint