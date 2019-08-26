#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// #include <mariadb/mysql.h>

#include "db_driver.h"
#include "huella.h"

int guardarHuella(unsigned char *buffer_data, size_t buffer_size, const char* rut)
{
    MYSQL mysql, *conn = NULL;
    MYSQL_STMT *STMT;
    char *sql = "UPDATE alumnos SET huella=\'";
    char *sql_end;
    char *stmt;
    int affected_rows = 0;
    int status = 0;
    sprintf(sql_end, "\' WHERE rut_alumno='%s'", rut);
    stmt = malloc(sizeof(char)*(strlen(sql) + strlen(sql_end) + buffer_size));
    sprintf(stmt, sql);
    mysql_init(&mysql);
    conn = mysql_real_connect(&mysql, HOST, USER, PASS, DB, PORT, UNIX_SOCKET, CLIENT_FLAG);
    if (!conn)
    {
        printf("Error!\n");
        exit(EXIT_FAILURE);
    }
    
    int size = mysql_real_escape_string(conn, stmt + strlen(sql), buffer_data, buffer_size);
    sprintf(stmt + strlen(sql) + buffer_size, sql_end);
    status = mysql_real_query(conn, stmt, (unsigned int) (strlen(stmt)));
    if (status != 0)
    {
        printf("Error: %s", mysql_error(conn));
        // mysql_close(conn);
        return -1;
    }
    affected_rows = mysql_affected_rows(conn);
    printf("%d filas afectadas en la BD\n", affected_rows);
    mysql_close(&mysql);
    return affected_rows;
}

//Test for prepared sql statements
int guardarHuellaPrep(unsigned char *buffer_data, size_t buffer_size, const char* rut)
{
    MYSQL mysql, *conn = NULL;
    MYSQL_STMT *STMT;
    MYSQL_BIND bind[2];
    mysql_init(&mysql);

    conn = mysql_real_connect(&mysql, HOST, USER, PASS, DB, PORT, UNIX_SOCKET, CLIENT_FLAG);

    /*** */
    char *stmt_str = "UPDATE alumnos SET huella=? WHERE rut_alumno = ?";
    int status = 0;
    const char *error;
    memset(bind, 0, sizeof(bind));
    STMT =  mysql_stmt_init(conn);
    status = mysql_stmt_prepare(STMT, stmt_str, strlen(stmt_str));
    if (status){
        error = mysql_stmt_error(STMT);
        printf("%s", *error);
        return 0;
    }

    //bindign params
    bind[0].buffer_type = MYSQL_TYPE_BLOB;
    bind[0].buffer = buffer_data;
    bind[0].buffer_length = buffer_size;
    bind[0].length = &buffer_size;
    bind[0].is_null = 0;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = rut;
    bind[1].buffer_length = strlen(rut);
    bind[1].is_null = 0;

    mysql_stmt_bind_param(STMT, bind);
    mysql_stmt_execute(STMT);
    my_ulonglong affected = 0;
    affected = mysql_stmt_affected_rows(STMT);
    mysql_stmt_close(STMT);
    
    
    /**/
}