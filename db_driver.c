#include "db_driver.h"
#include <stddef.h>

//Configuracion de variables de iniciacion de conexion
    MYSQL *conn, mysql;
    char *error;
    

MYSQL* db_connect()
{
    mysql_init(&mysql);
    conn = mysql_real_connect(&mysql, HOST, USER, PASS, DB, PORT , NULL, CLIENT_FLAG);
    return conn;

}

void db_disconnect(MYSQL *conn)
{
    if (conn != NULL)
    {
        mysql_close(conn);
        conn = NULL;
    }
}

char *get_error()
{
    return error;
}