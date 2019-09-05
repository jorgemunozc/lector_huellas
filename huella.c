#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mariadb/mysql.h>
#include <libfprint/fprint.h>

#include "db_driver.h"
#include "huella.h"

//Test for prepared sql statements
int guardarHuellaEnBD(unsigned char *buffer_data, size_t buffer_size, char* rut)
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
    bind[0].buffer_type = MYSQL_TYPE_LONG_BLOB;
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
    
    return 1;
    /**/
}

/**
 * Funcion para comparar huella escaneada contra la base de datos
 * @param 
 * */

int auntenticarHuella(struct fp_print_data *huellaEscaneada, struct fp_print_data *huellaEnBD)
{
    //Verificamos que ambas huellas existan
    if (!huellaEscaneada || !huellaEnBD)
    {
        printf("No se puede acceder a una de las huellas.\n");
        return -1;
    }

}