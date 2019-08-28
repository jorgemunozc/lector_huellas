/**
 * Modulo para recuperar las huellas desde la BD y luego compararlas con
 * un escaneo de la huella del usuario a identificar
 * */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <mariadb/mysql.h>
#include <string.h>
#include <libfprint/fprint.h>
#include "db_driver.h"
// #include "db_driver.h"

/**
 * Funcion verifica huella contra la base de datos
 * */
int verificarHuella(struct fp_dev *dev)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    MYSQL_FIELD *fields;
    MYSQL_ROW *alumnos;

    char *sql = "";
    struct fp_print_data *huella, **huellasBD, **ptr_aux;

    my_ulonglong num_rows;
    unsigned int num_fields;
    int affected_rows = 0;
    int i, state, resultado_ver;
    unsigned long *lenghts;
    size_t posHuella;//Posicion de huella que coincide con la escaneada
    const char *error;
    
    //Nos conectamos a la BD, si no lo consigue mostramos error
    printf("Conectando a Base de datos...\n");
    conn = db_connect();
    if (!conn)
    {
        error = mysql_error(conn);
        printf("FATAL: No se pudo conectar a la BD. Error: %s\n", error);
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
    //Guardamos el objeto con el resultado de la consulta
    res = mysql_store_result(conn);
    
    if (res)  //Existen filas retornadas en la consulta
    {
        num_rows = mysql_num_rows(res);//números de resultados retornados 
        num_fields =  mysql_num_fields(res);//numero de columnas que se solicitan de la tabla
        fields = mysql_fetch_fields(res);//metadata de las columnas recuperadas de la tabla

        //Solicitamos espacio para almacenar la info de los alumnos en memoria
        // huellasBD = (MYSQL*) malloc(sizeof(MYSQL_ROW) * num_rows);
        i = 0;
        ptr_aux = huellasBD;

        //Comenzamos a recorrer los resultado entregados por la consulta
        while (row = mysql_fetch_row(res))
        {
            /**
             * Recuperamos el tamaño de los campos seleccionados para saber
             * el tamaño de cada imagen
             * 
             **/
            lenghts = mysql_fetch_lengths(res);
            unsigned char *buffer;
            sprintf(buffer, row[0]);//Copiamos campo de huella a un buffer para poder descomprimirlo

            //Descomprimimos huella
            huella = fp_print_data_from_data(row[0], lenghts[0]);
            *huellasBD = huella;
            *huellasBD++;

            
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

    resultado_ver = fp_identify_finger(dev, huellasBD, &posHuella);
    switch (resultado_ver)
    {
        case FP_VERIFY_MATCH:
            printf("Huella coincide! :)");
            break;
        case FP_VERIFY_NO_MATCH:
            printf("Huella no coincide :(");
            break;
        case FP_VERIFY_RETRY:
			printf("Escaneo no se ralizó correctamente. Intente de nuevo.\n");
			break;
		case FP_VERIFY_RETRY_TOO_SHORT:
			printf("Retiro la huella muy pronto, intente de nuevo.\n");
			break;
		case FP_VERIFY_RETRY_CENTER_FINGER:
			printf("Ponga el dedo en el centro del sensor e intentelo de nuevo.\n");
			break;
		case FP_VERIFY_RETRY_REMOVE_FINGER:
			printf("Por favor remueva el dedo del sensor e intentelo de nuevo.\n");
			break;
        default:
            printf("Algun error sucedió :*");
            break;
    }
    printf("\n");
    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}
