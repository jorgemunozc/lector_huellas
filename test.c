#include <stdio.h>
#include <stdlib.h>
#include <mariadb/mysql.h>


#include <libfprint/fprint.h>
#include "db_config.h"

struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs)
{
	struct fp_dscv_dev *ddev = discovered_devs[0];
	struct fp_driver *drv;
	if (!ddev)
		return NULL;
	
	drv = fp_dscv_dev_get_driver(ddev);
	// printf("Found device claimed by %s driver\n", fp_driver_get_full_name(drv));
	return ddev;
}
/**
 * ===============================
 * DB connection functions
 * ===============================
 **/
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


int main(void)
{
	int r = 1;
	struct fp_dscv_dev *ddev;
	struct fp_dscv_dev **discovered_devs;
	struct fp_dev *dev;
	struct fp_img *img = NULL;
	struct fp_print_data *bd_img;

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

	
	// setenv ("G_MESSAGES_DEBUG", "all", 0);
	setenv ("LIBUSB_DEBUG", "3", 0);

	//Inicializacion de libfprint
	r = fp_init();
	if (r < 0) {
		fprintf(stderr, "Failed to initialize libfprint\n");
		exit(1);
	}

	discovered_devs = fp_discover_devs();
	if (!discovered_devs) {
		fprintf(stderr, "Could not discover devices\n");
		goto out;
	}

	ddev = discover_device(discovered_devs);
	if (!ddev) {
		fp_dscv_devs_free(discovered_devs);
		fprintf(stderr, "No devices detected.\n");
		goto out;
	}

	dev = fp_dev_open(ddev);
	fp_dscv_devs_free(discovered_devs);
	if (!dev) {
		fprintf(stderr, "Could not open device.\n");
		goto out;
	}

	if (!fp_dev_supports_imaging(dev)) {
		fprintf(stderr, "this device does not have imaging capabilities.\n");
		goto out_close;
	}

	printf("Dispositivo conectado. Escanea tu dedo.\n");

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
            char finger_data[lenghts[0]];
            sprintf(finger_data, row[0]);
			bd_img = fp_print_data_from_data(finger_data, lenghts[0]);
			if (fp_verify_finger(dev, bd_img) == FP_VERIFY_MATCH)
			{
				printf("Huella encontrada, pertence a %s\n", row[1]);
				goto out_close;
			}
            
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
	printf("Huella no encontrada en BD\n");
	goto out_close;
	/* r = fp_dev_img_capture(dev, 0, &img);
	if (r) {
		fprintf(stderr, "image capture failed, code %d\n", r);
		goto out_close;
	} */
	r = fp_dev_img_capture(dev, 0, &img);
		if (r) {
			printf("No se pudo capturar huella, intentelo de nuevo\n");
		}
	
	printf("Estandarizando imagen capturada...\n");
	fp_img_standardize(img);
	printf("Imagen estandarizada\n");
	/* r = fp_img_save_to_file(img, "finger.pgm");
	if (r) {
		fprintf(stderr, "img save failed, code %d\n", r);
		goto out_close;
	} *//* 

	r = fp_img_save_to_file(img, "finger_standardized.pgm");
	fp_img_free(img);
	if (r) {
		fprintf(stderr, "standardized img save failed, code %d\n", r);
		goto out_close;
	} */

	r = 0;
out_close:
	fp_dev_close(dev);
	mysql_free_result(res);
    mysql_close(conn);
out:
	fp_exit();
	return r;
}