#ifndef HUELLA_H
#define HUELLA_H

#include <mariadb/mysql.h>
#include <libfprint/fprint.h>

int guardarHuellaEnBD(unsigned char *buffer, size_t buffer_size, char *rut);

#endif