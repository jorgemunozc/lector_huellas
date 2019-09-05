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
#include "libs/libfprint/fp_internal.h"
#include "libs/libfprint/fprint.h"

// #include "libs/libfprint/fp_internal.h"
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
    // bool *errorBlob, *errorString;
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
        huellasBD = (MYSQL*) malloc(sizeof(MYSQL_ROW) * num_rows);
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
    MYSQL_FIELD *fields; 
    MYSQL_BIND bind[2];

    char *sql = "";
    char *error;
    char *nombres;
    const unsigned long MAX_BUFFER_SIZE = 65535; //16MB
    const unsigned long STR_SIZE = 40;
    unsigned char buffer_data[MAX_BUFFER_SIZE];
    unsigned long str_len;
    unsigned long buffer_real_len;
    int status = 1, i;
    int res_verificacion = 0;
    int val = 0;
    size_t posHuella = -1;
    char nombre[STR_SIZE];
    my_bool errorBlob;
    my_bool errorString;

    struct fp_print_data *huella, **huellasBD, **ptr_aux;

    unsigned int num_rows; 

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
    fields = mysql_fetch_fields(metadata_res);
    // buffer_data = malloc(sizeof(char) * fields[0].length);
    if (mysql_stmt_execute(stmt))
    {
        printf("No se pudo ejecutar la consulta.\n");
        return 0;
    }

    //Configuracion de campos que se van a recuperar de la BD
    memset(bind, 0, sizeof(bind));
    
    bind[0].buffer_type = fields[0].type;
    bind[0].buffer = (char *)buffer_data;
    bind[0].buffer_length = MAX_BUFFER_SIZE;
    bind[0].length = &buffer_real_len;
    bind[0].error = &errorBlob;


    bind[1].buffer_type = fields[1].type;
    bind[1].buffer = (char *)nombre;
    bind[1].buffer_length = STR_SIZE;
    bind[1].length = &str_len;
    bind[1].error = &errorString;

    //Amarramos las variables bind a la consulta preparada
    // metadata_res = mysql_stmt_result_metadata(stmt);
    status = mysql_stmt_bind_result(stmt, bind);
    if (status != 0)
    {
        error = mysql_stmt_error(stmt);
        printf("ERROR: %s\n", error);
    }
    // mysql_stmt_store_result(stmt);
    // num_rows = (unsigned int) mysql_stmt_num_rows(stmt);
    //REservamos espacio para almacenar el arreglo de huellas obtenidas desde la BD
    // huellasBD = malloc(sizeof(struct fp_print_data*) * num_rows);
    // nombres = malloc(sizeof(char *) * (int)num_rows);
    // char *str;
    
    // str = (char*)calloc(num_rows, sizeof(char));
    //Recuperamos las filas retornadas por la consulta
    // ptr_aux = huellasBD;
    i = 0;
    while (1)
    {
        // buffer_data = malloc(sizeof(char) * MAX_BUFFER_SIZE);
        status = mysql_stmt_fetch(stmt);
        if ( status == 1 || status == MYSQL_NO_DATA)
        {
            break;
        }
        else if (status == MYSQL_DATA_TRUNCATED)
        {
            printf("Informacion de campo incompleta.\n");
            break;
        }
        //Descomprimimos huella
        huella = fp_print_data_from_data(buffer_data, buffer_real_len);
        // huellasBD[i] = huella;
        // nombres[i] = nombre;
        // strcpy(nombres[i], nombre);
        i++;
        res_verificacion = fp_verify_finger(dev, huella);
        switch (res_verificacion)
    {
        case FP_VERIFY_MATCH:
            printf("Huella coincide! :)\n");
            printf("Pertenece a: %s", nombre);
            break;
        case FP_VERIFY_NO_MATCH:
            printf("Huella no coincide :(");
            break;
        case FP_VERIFY_RETRY:
			printf("Escaneo no se ralizó correctamente. Intente de nuevo.");
			break;
		case FP_VERIFY_RETRY_TOO_SHORT:
			printf("Retiro la huella muy pronto, intente de nuevo.");
			break;
		case FP_VERIFY_RETRY_CENTER_FINGER:
			printf("Ponga el dedo en el centro del sensor e intentelo de nuevo.");
			break;
		case FP_VERIFY_RETRY_REMOVE_FINGER:
			printf("Por favor remueva el dedo del sensor e intentelo de nuevo.");
			break;
        default:
            printf("Algun error sucedió.");
            break;
    }
    printf("\n");
    }
    /**
     * Comparacion de la huella almacenada en la BD con la escaneada
     * */
    // res_verificacion = fp_identify_finger(dev, huellasBD, &posHuella);
    // switch (res_verificacion)
    // {
    //     case FP_VERIFY_MATCH:
    //         printf("Huella coincide! :)\n");
    //         printf("Pertenece a: %s", *nombres[posHuella]);
    //         break;
    //     case FP_VERIFY_NO_MATCH:
    //         printf("Huella no coincide :(");
    //         break;
    //     case FP_VERIFY_RETRY:
	// 		printf("Escaneo no se ralizó correctamente. Intente de nuevo.");
	// 		break;
	// 	case FP_VERIFY_RETRY_TOO_SHORT:
	// 		printf("Retiro la huella muy pronto, intente de nuevo.");
	// 		break;
	// 	case FP_VERIFY_RETRY_CENTER_FINGER:
	// 		printf("Ponga el dedo en el centro del sensor e intentelo de nuevo.");
	// 		break;
	// 	case FP_VERIFY_RETRY_REMOVE_FINGER:
	// 		printf("Por favor remueva el dedo del sensor e intentelo de nuevo.");
	// 		break;
    //     default:
    //         printf("Algun error sucedió.");
    //         break;
    // }
    // printf("\n");
    mysql_free_result(metadata_res);
    db_disconnect(conn);
    return 0;

}

struct fp_print_data* capturarHuella(struct fp_dev *dev)
{
    struct fp_img *ImgHuella;
    printf("Comenzando captura de dedo indice...\n");
    if (fp_dev_img_capture(dev, 0, &ImgHuella))
    {
        
    }
}