/**
 * Modulo para recuperar las huellas desde la BD y luego compararlas con
 * un escaneo de la huella del usuario a identificar
 * */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <mariadb/mysql.h>
#include <string.h>
#include "db_config.h"
// #include "db_driver.h"

MYSQL *conn , mysql;
const char *error;

//HELPER FUNCTIONS
int db_connect()
{
    mysql_init(&mysql);
    conn = mysql_real_connect(&mysql, HOST, USER, PASS, DB, PORT , NULL, CLIENT_FLAG);
    if ( conn == NULL)
    {
        return -1;
    }
    return 0;

}

void db_disconnect()
{
    if (conn != NULL)
    {
        mysql_close(conn);
        conn = NULL;
    }
}

const char *get_error()
{
    return error;
}


int main()
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    MYSQL_FIELD *fields;
    MYSQL_ROW *alumnos;

    char *sql = "";
    char *huella;

    my_ulonglong num_rows;
    unsigned int num_fields;
    int affected_rows = 0;
    int i, state;
    unsigned long *lenghts;
    
    //Nos conectamos a la BD, si no lo consigue mostramos error
    if ( db_connect() != 0)
    {
        error = mysql_error(conn);
        printf("No se pudo conectar a la BD. Error: %s\n", error);
        exit(0);
    }
    
    sql = "SELECT huella, nombres_alumno FROM alumnos";
    
    //Realizamos la consulta
    state = mysql_query(conn, sql);
    if (state != 0)
    {
        error = mysql_error(conn);
        printf("No se pudo realizar la consulta. Error: %s\n", error);
        return 0;
    }
    res = mysql_store_result(conn);
    
    if (res)  //Existen filas retornadas en la consulta
    {
        num_rows = mysql_num_rows(res);
        num_fields =  mysql_num_fields(res);
        fields = mysql_fetch_fields(res);
        //Solicitamos espacio para almacenar la info de los alumnos en memoria
        alumnos = malloc(sizeof(MYSQL_ROW) * num_rows);
        i = 0;
        while (row = mysql_fetch_row(res))
        {
            /**
             * REcuperamos el tamaño de los campos seleccionados para saber
             * el tamaño de cada imagen
             * 
             **/
            lenghts = mysql_fetch_lengths(res);
            char buffer[lenghts[0]];
            sprintf(buffer, row[0]);
            
            /* for (i = 0; i < num_fields; i++)
            {
                if (fields[i].name == "huella")
                {
                    huella = malloc(sizeof(char)*lenghts[0]);
                    huella = row[i];
                }
            }
            printf("\n"); */
        }
    }
    else  // mysql_store_result() returned nothing; should it have?
    {
        if(mysql_field_count(conn) == 0)
        {
            // query does not return data
            // (it was not a SELECT)
        }
        else // mysql_store_result() should have returned data
        {
            fprintf(stderr, "Error: %s\n", mysql_error(conn));
        }
    }
    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}