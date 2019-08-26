#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// #include <mariadb/mysql.h>

#include "db_config.h"
#include "huella.h"

int guardarHuella(unsigned char *buffer_data, size_t buffer_size, const char* rut)
{
    MYSQL *conn = NULL;
    char *sql = "UPDATE alumnos SET huella=";
    char *stmt;
    stmt = (char *) malloc(sizeof(char)*(strlen(sql) + strlen(rut) + buffer_size));
    int affected_rows = 0;
    int status = 0;
    sprintf(stmt, sql, rut);
    mysql_init(conn);
    conn = mysql_real_connect(conn, HOST, USER, PASS, DB, PORT, UNIX_SOCKET, CLIENT_FLAG);
    if (!conn)
    {
        printf("Error!\n");
        exit(EXIT_FAILURE);
    }
    status = mysql_real_query(conn, stmt, (unsigned int) (strlen(stmt)));
    if (status != 0)
    {
        return -1;
    }
    affected_rows = mysql_affected_rows(conn);
    printf("%d filas afectadas en la BD\n");
    return affected_rows;
    // mysql_real_escape_string(conn, )
}