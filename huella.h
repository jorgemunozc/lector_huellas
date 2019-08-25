#ifndef HUELLA_H
#define HUELLA_H

#include <mariadb/mysql.h>
#include <libfprint/fprint.h>
int guardarHuella(unsigned char*, size_t);
#endif