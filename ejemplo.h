/*
 * ejemplo.h: Es el archivo de cabecera asociado a ejemplo.c. Define las macros
 * globales (como MAXNOMBRE) y agrupa los prototipos de todas las funciones de
 * comandos implementadas en ejemplo.c para que sean visibles desde el resto de módulos.
 */

#ifndef EJEMPLO_H
#define EJEMPLO_H

#include <stdio.h>
#include <stdlib.h> // Control de memoria (funciones como exit)
#include <unistd.h> // Funciones de POSIX del SO (llamadas como fork, execv, getpid, chdir, close).
#include <string.h> // Manipulación de cadenas de caracteres (strcmp, strncmp, strncpy, strtok).
#include <sys/types.h> // Define tipos de datos utilizados en llamadas al sistema (como pid_t).
#include <sys/stat.h> // Estructuras y macros para consultar información de archivos y permisos (usado con stat o lstat).
#include <fcntl.h> // Constantes para la apertura de ficheros (como cr, rw, etc.).
#include <time.h> // Necesario para el date

#include "path.h"

#define MAXNOMBRE 1024

//  Estructura para almacenar los ficheros abiertos
struct fich_abierto {
    int descriptor; // Nº de descriptor de fichero devuelto por el SO
    char nombre[MAXNOMBRE]; // Nombre o ruta del fichero abierto
    int modo; // Ej. lectura, escritura...
};

void Cmd_fin (char * arg[]);
void Cmd_autores(char *arg[]);
void Cmd_exec (char *arg[]); // Ejecuta un comando externo de forma directa.
void Cmd_splano (char *arg[]); // Ejecuta un comando en segundo plano
void Cmd_pplano (char *arg[]); // en el primero
void Cmd_chdir (char * arg[]); // Muestra directorio actual
void Cmd_pwd(char * arg[]); // uUestra la ruta del directorio actual
void Cmd_pid (char * arg[]);

// Sobre el PATH
void Cmd_path (char * arg[]);
void Cmd_importpath (char *arg[]);
void Cmd_where (char *args[]); // Localiza dónde se encuentra un ejecutable dentro del PATH.

// Añadido
void Cmd_date(char *arg[]);
void Cmd_sysinfo(char *arg[]); // Muestra info de la máquina y del SO (uname)
void Cmd_help(char *arg[]); // Muestra la ayuda general del shell o info. detallada de un comando
void Cmd_open(char *arg[]); //Abre un fichero con unos modos específicos y lo añade a la lista interna.
void Cmd_close(char *arg[]); // Cierra un descriptor de fichero y lo elimina de la lista interna.
void Cmd_listopen(char *arg[]); // Lista todos los ficheros que el shell mantiene abiertos actualmente.



#endif
