/**
 * Modulo para enrolar huella y luego guardarla en BD
 * 
 * TODO: guardado en BD
 * */


#include <stdio.h>
#include <stddef.h>
#include <libfprint/fprint.h>
#include <unistd.h>

#include "huella.h"
#include "verificar.h"


struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs)
{
	struct fp_dscv_dev *ddev = discovered_devs[0];
	struct fp_driver *drv;
	if (!ddev)
		return NULL;
	
	drv = fp_dscv_dev_get_driver(ddev);
	printf("Found device claimed by %s driver\n", fp_driver_get_full_name(drv));
	return ddev;
}

/**
 * Funcion recibe el dispositivo en el cual enrolar la imagen, y
 * nos devuelve la información de la huella que fue enrolada exitosamente,
 * si aplica, de lo contrario retorna NULL
 * 
 * */
struct fp_print_data *enrolar(struct fp_dev *dev)
{
    int status = 0, i = 1;
    struct fp_print_data *enrolled_print = NULL;
    int n_stages = fp_dev_get_nr_enroll_stages(dev);

    printf("Escanea tu huella %d veces, por favor\n", n_stages);


    do
    {
        printf("Comenzando captura de huella...\n");
        printf("Escaneando huella %d/%d...\n", i, n_stages);
        //Comenzamos scaneo
        status = fp_enroll_finger(dev, &enrolled_print);

        switch (status) {
		case FP_ENROLL_COMPLETE:
			printf("Captura completada.\nRetire su dedo.\n");
			break;
		case FP_ENROLL_FAIL:
			printf("Error en captura.\n");
			return NULL;
		case FP_ENROLL_PASS:
			printf("Huella escaneada exitosamente.\n");
            i++;
			break;
		case FP_ENROLL_RETRY:
			printf("No se capturó huella. Inténtelo de nuevo.\n");
			break;
		case FP_ENROLL_RETRY_TOO_SHORT:
			printf("Sacó muy rápido su dedo del scanner, por favor inténtelo de nuevo.\n");
			break;
		case FP_ENROLL_RETRY_CENTER_FINGER:
			printf("Inténtelo de nuevo.\n");
			break;
		case FP_ENROLL_RETRY_REMOVE_FINGER:
			printf("Escaneo falló, por favor remueva su dedo e inténtelo de nuevo.\n");
			break;
		}


    } while (status != FP_ENROLL_COMPLETE);

    return enrolled_print;
    
}

int main()
{
    int r = 1;
    int nr_stages = 0;
	struct fp_dscv_dev *ddev;
	struct fp_dscv_dev **discovered_devs;
	struct fp_dev *dev;
	struct fp_print_data *info_huella;

    unsigned char *buffer;
    size_t buffer_size;

    r = fp_init();
    //Buscamos dispositivos compatibles
    discovered_devs = fp_discover_devs();
    if (!discovered_devs) {
		fprintf(stderr, "No se pudieron detectar dispositivos.\n");
		return 0;
	}
    //Seleccionamos dispositivo con el que queremos trabajar
    ddev = discover_device(discovered_devs);
    if (!ddev)
    {
        fprintf(stderr, "No se detectaron huelleros.\n");
        return 0;
    }

    /**
     * Una vezz descubierto que el huellero esta conectado y es compatible,
     * procedemos a abrir el dispositivo para poder operarlo
     * */
    dev = fp_dev_open(ddev);

    //Liberamos la memoria de la lista de dispositivo, no la usaremos más
    fp_dscv_devs_free(discovered_devs);

    if (!dev)
    {
        fprintf(stderr, "No se pudo abrir el huellero.");
        return 0;
    }
    
    info_huella = enrolar(dev);
    
    if (!info_huella)
    {
        printf("Saliendo....\n");
        return 0;
    }

    printf("Huella enrolada exitosamente en memoria.\n");
    buffer_size = fp_print_data_get_data(info_huella, &buffer);
    fp_print_data_free(info_huella);

    //Guardamos huella en BD
    // guardarHuellaEnBD(buffer, buffer_size, "1234567-9");
    sleep(1);
    printf("Procediendo a autentificar huella, por favor ponga su dedo indice...\n");
    verificarHuellaPrep(dev);
    sleep(1);
    
    // /**
    //  * Pequeño test para verificar si se reconoce la huella ingresada
    //  * */
    // if (!fp_dev_supports_print_data(dev, info_huella))
    // {
    //     printf("Dispositivo no compatible con la captura de huella");
    // }

    // int verif_status = fp_verify_finger(dev, info_huella);
    // switch (verif_status)
    // {
    // case FP_VERIFY_MATCH:
    //     printf("Huella coincide! :)");
    //     break;
    // case FP_VERIFY_NO_MATCH:
    //     printf("Huella no coincide :(");
    //     break;
    // default:
    //     printf("Algun error sucedio :*");
    //     break;
    // }
    // printf("\n");
    //Liberamos la memoria asociada a la huella
    //TEst para verificar huella en BD

    fp_dev_close(dev);
    fp_exit();


    return 0;

}
