#include <mariadb/mysql.h>

#ifndef DB_DRIVER_H
#define DB_DRIVER_H

#include <stddef.h>

//Configuracion de variables de iniciacion de conexion
    #define HOST "localhost"
    #define DB "db_name"
    #define USER "user_name"
    #define PASS "password"
    #define PORT  3306
    #define UNIX_SOCKET NULL
    #define CLIENT_FLAG 0

    
//metodos
int db_init(void);
MYSQL *db_connect(void);
void db_disconnect(MYSQL *conn);

#endif
