CC = gcc
CFLAGS = -g 
FPRINTFLAGS = -L="/usr/include/libfprint" -lfprint
MYSQLFLAGS = `mysql_config --cflags --libs`

main:
	${CC} ${CFLAGS} ${FPRINTFLAGS} main.c -o fingerprint

connectdb:
	${CC} ${CFLAGS} ${MYSQLFLAGS} connectdb.c db_config.c -o testDB

enroll:
	${CC} ${CFLAGS} ${FPRINTFLAGS} enrolar.c -o enrolar

all:
	${CC} ${CFLAGS} ${FPRINTFLAGS} ${MYSQLFLAGS} main.c db_config.c -o verifyprint