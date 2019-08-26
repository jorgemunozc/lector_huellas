#ifndef HUELLA_H
#define HUELLA_H

#include <mariadb/mysql.h>
#include <libfprint/fprint.h>
int guardarHuella(unsigned char *buffer, size_t buffer_size, const char *rut);
#endif