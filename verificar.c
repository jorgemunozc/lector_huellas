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
    

    /**
     * Comparacion de la huella almacenada en la BD con la escaneada
     * */
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


int verificarHuellaPrep(struct fp_dev *dev)
{ 
    MYSQL *conn;
    MYSQL_RES *metadata_res;
    MYSQL_STMT *stmt;
    MYSQL_BIND bind[2];

    char *sql = "";
    int status = 1;
    char *nombre = "";
    char *error;
    unsigned char *buffer_data;
    unsigned long MAX_BUFFER_SIZE = 16777215; //16MB
    MYSQL_FIELD *fields; 
    int val = 0;

    memset(bind, 0, sizeof(bind));
    conn = db_connect();
    if (!conn)
    {
        error = mysql_error(conn);
        printf("FATAL: No se pudo conectar a la BD. Error: %s\n", error);
        exit(0);
    }
    //Iniciamos la estructura para la consulta preparada
    stmt = mysql_stmt_init(conn);
    sql = "SELECT huella, nombres_alumno FROM alumnos";
    //Preparamos la consulta en el servidor
    status = mysql_stmt_prepare(stmt, sql, strlen(sql));
    if (status != 0){
        error = mysql_stmt_error(stmt);
        printf("ERROR: %s\n", *error);
        return 0;
    }
    //Actualizamos el campo que nos da el tamaño maximo de la columna dentro del resultado con
    //la metadata de las columnas
    mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, (void *)&val);
    metadata_res = mysql_stmt_result_metadata(stmt);
    // mysql_stmt_store_result(stmt);
    fields = mysql_fetch_fields(metadata_res);

    //Configuracion de campos que se van a recuperar de la BD
    bind[0].buffer_type = MYSQL_TYPE_BLOB;
    bind[0].buffer = buffer_data;
    bind[0].buffer_length = fields[0].max_length;


    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = nombre;
    bind[1].buffer_length = fields[1].max_length;

    mysql_stmt_execute(stmt);
    //Amarramos las variables bind a la consulta preparada
    status = mysql_stmt_bind_result(stmt, bind);
    if (status != 0)
    {
        error = mysql_stmt_error(stmt);
        printf("ERROR: %s\n", error);
    }
    //Recuperamos las filas retornadas por la consulta
    while (mysql_stmt_fetch(stmt))
    {
        fprintf(stdout, "%s\n", buffer_data, fields[0].max_length);
    }

    mysql_free_result(metadata_res);
    db_disconnect(conn);
    return 0;

}