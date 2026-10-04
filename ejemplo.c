/**
 * ejemplo.c: Contiene la implementación de la gran mayoría de los comandos
 * internos del shell (como chdir, pwd, pid, path, where, exec, etc.) y las
 * funciones auxiliares para la gestión de procesos (fork, waitpid) y control
 * de la ejecución en primer o segundo plano (splano, pplano).
 */
#include "ejemplo.h"


void AniadirAlPath(char *dir)
{
    if (dir==NULL)
        PathPrint();
    else if (PathAdd(dir)==-1)
        perror("Imposible aniadir");
}

void EliminarDelPath(char *dir)
{
    if (dir==NULL)
        PathPrint();
    else if (PathDel(dir)==-1)
        perror("Imposible eliminar");
}


void MostrarDirActual()
{
   char dir[MAXNOMBRE];

    if (getcwd(dir,MAXNOMBRE)==NULL)
	perror("Imposible obtener directorio");
    else
       printf ("%s\n",dir);
}

int ComprobarSegundoPlano (char *tr[])
{
    int i;
    for (i=0; tr[i]!=NULL;i++)
        if (!strcmp(tr[i],"&")){ /*& indica segundo plano*/
            tr[i]=NULL;         /*es el ultimo argumento*/
            return i;       /*si solo hay un & no se ejecuta nada en pplano*/
            }
    return 0;
}


void Proceso (char *tr[], int splano)
{
   pid_t pid;
   void Cmd_exec (char **);
   int background=splano || ComprobarSegundoPlano(tr);
   if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
        }
  if (pid==0){  /*proceso hijo*/
    Cmd_exec (tr);
    exit(255); /*por si falla exec*/
    }
  if (!background)
    waitpid(pid,NULL,0);
}

/*********************************************/
/*************COMANDOS DEL SHELL************************/
void Cmd_fin (char * arg[])  /*todos los cmd_ comparten prototipo*/
{                            /*reciben los mismos parametros aunque no los usen*/
    exit(0);
}

//Editado
void Cmd_autores(char *arg[])
{
    char *nombres = "Nombre Alumno 1 & Nombre Alumno 2";
    char *logins = "login1 & login2";

    if (arg[0] == NULL) {
        printf("Autores: %s (%s)\n", nombres, logins);
    } else if (!strcmp(arg[0], "-l")) {
        printf("%s\n", logins);
    } else if (!strcmp(arg[0], "-n")) {
        printf("%s\n", nombres);
    } else {
        printf("Uso: autores [-l|-n]\n");
    }
}

void Cmd_exec (char *arg[])
{
  if (execv(Ejecutable(arg[0]),arg)==-1)
	perror ("Imposible ejecutar");
}

void Cmd_splano (char *arg[])
{
  Proceso (arg,1);
}
void Cmd_pplano (char *arg[])
{
  Proceso(arg,0);
}

void Cmd_chdir (char * arg[])
{
   if (arg[0]==NULL)
      MostrarDirActual();
   else if (chdir(arg[0])==-1)
      perror("Imposible cambiar directorio");
}

void Cmd_pwd(char * arg[])
{
    MostrarDirActual();
}

void Cmd_pid (char * arg[])
{
    if (arg[0]==NULL)
        printf ("El pid del proceso es %d\n",(int) getpid());
    else
        if (!strcmp (arg[0],"-p"))
            printf ("El pid del proceso padre es %d\n",(int) getppid());
}

void Cmd_where (char *args[])
{
    if (args[0]==NULL)
        printf ("uso: where ejecutable. Indica donde ejecutable está en el path\n");
    else
        printf ("%s\n",Ejecutable (args[0]));
}


void Cmd_path(char *arg[])
{
    if (arg[0]==NULL)
        PathPrint();
    else if (!strcmp(arg[0],"-add"))
        AniadirAlPath (arg[1]);
    else if (!strcmp(arg[0],"-del"))
        EliminarDelPath (arg[1]);
    else if (!strcmp(arg[0],"-show"))
        PathPrint ();
    else if (!strcmp(arg[0],"-clear"))
        PathClear ();
    else if (!strcmp(arg[0],"-import"))
        PathAddPath ();
    else printf ("Opciones validas: -add|-del|-show|-clear|-import\n");
}

void Cmd_importpath (char *arg[])
{
    PathAddPath();
}

/*********************************************/
/************* NUEVOS COMANDOS P1 ************/
/*********************************************/

#define MAX_FICH_ABIERTOS 64
static struct fich_abierto ficheros_abiertos[MAX_FICH_ABIERTOS];

void AnadirFicheroAbierto(int desc, char *nombre, int modo) {
    int i;
    for (i = 0; i < MAX_FICH_ABIERTOS; i++) {
        if (ficheros_abiertos[i].descriptor == -1) {
            ficheros_abiertos[i].descriptor = desc;
            strncpy(ficheros_abiertos[i].nombre, nombre, MAXNOMBRE - 1);
            ficheros_abiertos[i].nombre[MAXNOMBRE - 1] = '\0';
            ficheros_abiertos[i].modo = modo;
            return;
        }
    }
}

void EliminarFicheroAbierto(int desc) {
    int i;
    for (i = 0; i < MAX_FICH_ABIERTOS; i++) {
        if (ficheros_abiertos[i].descriptor == desc) {
            ficheros_abiertos[i].descriptor = -1;
            ficheros_abiertos[i].nombre[0] = '\0';
            return;
        }
    }
}

void InicializarFicherosAbiertos(void) {
    int i;
    for (i = 0; i < MAX_FICH_ABIERTOS; i++) {
        ficheros_abiertos[i].descriptor = -1;
    }
    AnadirFicheroAbierto(0, "stdin", O_RDONLY);
    AnadirFicheroAbierto(1, "stdout", O_WRONLY);
    AnadirFicheroAbierto(2, "stderr", O_WRONLY);
}

void Cmd_date(char *arg[]) {
    time_t t;
    struct tm *info;
    char buffer[80];

    time(&t);
    info = localtime(&t);

    if (arg[0] == NULL) {
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", info);
        printf("%s\n", buffer);
    } else if (!strcmp(arg[0], "-d")) {
        strftime(buffer, sizeof(buffer), "%d/%m/%Y", info);
        printf("%s\n", buffer);
    } else if (!strcmp(arg[0], "-t")) {
        strftime(buffer, sizeof(buffer), "%H:%M:%S", info);
        printf("%s\n", buffer);
    } else {
        printf("Uso: date [-d | -t]\n");
    }
}

void Cmd_sysinfo(char *arg[]) {
    struct utsname info;
    if (uname(&info) == -1) {
        perror("Imposible obtener sysinfo");
        return;
    }
    printf("Sistema: %s\n", info.sysname);
    printf("Nodename: %s\n", info.nodename);
    printf("Release: %s\n", info.release);
    printf("Version: %s\n", info.version);
    printf("Machine: %s\n", info.machine);
}

void Cmd_open(char *arg[]) {
    int i, flags = 0;
    int df;

    if (arg[0] == NULL) {
        Cmd_listopen(NULL);
        return;
    }

    for (i = 1; arg[i] != NULL; i++) {
        if (!strcmp(arg[i], "cr")) flags |= O_CREAT;
        else if (!strcmp(arg[i], "ap")) flags |= O_APPEND;
        else if (!strcmp(arg[i], "ex")) flags |= O_EXCL;
        else if (!strcmp(arg[i], "ro")) flags |= O_RDONLY;
        else if (!strcmp(arg[i], "rw")) flags |= O_RDWR;
        else if (!strcmp(arg[i], "wo")) flags |= O_WRONLY;
        else if (!strcmp(arg[i], "tr")) flags |= O_TRUNC;
    }

    if ((flags & O_RDONLY) == 0 && (flags & O_WRONLY) == 0 && (flags & O_RDWR) == 0) {
        flags |= O_RDONLY;
    }

    df = open(arg[0], flags, 0666);
    if (df == -1) {
        perror("Imposible abrir fichero");
    } else {
        printf("Fichero abierto con descriptor: %d\n", df);
        AnadirFicheroAbierto(df, arg[0], flags);
    }
}

void Cmd_close(char *arg[]) {
    int df;
    if (arg[0] == NULL) {
        printf("Uso: close descriptor\n");
        return;
    }
    df = atoi(arg[0]);
    if (close(df) == -1) {
        perror("Imposible cerrar descriptor");
    } else {
        EliminarFicheroAbierto(df);
        printf("Fichero con descriptor %d cerrado.\n", df);
    }
}

void Cmd_listopen(char *arg[]) {
    int i;
    printf("Ficheros abiertos:\n");
    for (i = 0; i < MAX_FICH_ABIERTOS; i++) {
        if (ficheros_abiertos[i].descriptor != -1) {
            printf("Descriptor: %d | Nombre: %s\n", ficheros_abiertos[i].descriptor, ficheros_abiertos[i].nombre);
        }
    }
}

void Cmd_help(char *arg[])
{
    if (arg[0] == NULL) {
        printf("Comandos internos disponibles:\n");
        printf(" - fin, exit, quit, bye: Sale del shell.\n");
        printf(" - date [-d | -t]: Muestra la fecha y/o hora actual.\n");
        printf(" - pid [-p]: Muestra el PID del proceso o de su padre.\n");
        printf(" - pwd: Muestra el directorio de trabajo actual.\n");
        printf(" - chdir [dir]: Muestra o cambia el directorio de trabajo.\n");
        printf(" - autores [-l | -n]: Muestra los autores del shell.\n");
        printf(" - exec [prog]: Ejecuta un programa reemplazando el shell.\n");
        printf(" - pplano [prog]: Ejecuta un programa en primer plano.\n");
        printf(" - splano [prog]: Ejecuta un programa en segundo plano.\n");
        printf(" - sysinfo: Muestra información del sistema.\n");
        printf(" - path [-add|-del|-show|-clear|-import]: Gestiona el PATH.\n");
        printf(" - importpath: Importa la variable de entorno PATH del SO.\n");
        printf(" - where [ejecutable]: Busca la ruta de un ejecutable en el PATH.\n");
        printf(" - open [fichero] [modos]: Abre un fichero y lo añade a la lista.\n");
        printf(" - close [df]: Cierra un descriptor de fichero abierto.\n");
        printf(" - listopen: Lista todos los ficheros abiertos por el shell.\n");
        printf(" - help [cmd]: Muestra esta ayuda general o información detallada de un comando.\n");
    } else {
        if (!strcmp(arg[0], "date")) {
            printf("Uso: date [-d | -t]\n");
            printf("  Muestra la fecha y hora actual.\n");
            printf("  -d: Muestra únicamente la fecha.\n");
            printf("  -t: Muestra únicamente la hora.\n");
        } else if (!strcmp(arg[0], "pid")) {
            printf("Uso: pid [-p]\n");
            printf("  Muestra el PID del proceso del shell.\n");
            printf("  -p: Muestra el PID del proceso padre del shell.\n");
        } else if (!strcmp(arg[0], "autores")) {
            printf("Uso: autores [-l | -n]\n");
            printf("  Muestra los nombres y logins de los autores del shell.\n");
            printf("  -l: Muestra solo los logins.\n");
            printf("  -n: Muestra solo los nombres.\n");
        } else if (!strcmp(arg[0], "chdir")) {
            printf("Uso: chdir [dir]\n");
            printf("  Cambia el directorio de trabajo actual del shell a 'dir'.\n");
            printf("  Sin argumentos muestra el directorio actual.\n");
        } else if (!strcmp(arg[0], "pwd")) {
            printf("Uso: pwd\n");
            printf("  Muestra la ruta absoluta del directorio de trabajo actual.\n");
        } else if (!strcmp(arg[0], "exec")) {
            printf("Uso: exec [comando [args...]]\n");
            printf("  Ejecuta el comando reemplazando la imagen del proceso shell actual mediante execv.\n");
        } else if (!strcmp(arg[0], "pplano")) {
            printf("Uso: pplano [comando [args...]]\n");
            printf("  Ejecuta un programa externo en primer plano (el shell espera a que finalice).\n");
        } else if (!strcmp(arg[0], "splano")) {
            printf("Uso: splano [comando [args...]]\n");
            printf("  Ejecuta un programa externo en segundo plano.\n");
        } else if (!strcmp(arg[0], "path")) {
            printf("Uso: path [-add dir | -del dir | -show | -clear | -import]\n");
            printf("  Gestiona la lista interna del PATH del shell.\n");
            printf("  -add dir: Añade un directorio al PATH.\n");
            printf("  -del dir: Elimina un directorio del PATH.\n");
            printf("  -show: Muestra los directorios en el PATH.\n");
            printf("  -clear: Vacía la lista de directorios del PATH.\n");
            printf("  -import: Importa la variable de entorno PATH del sistema.\n");
        } else if (!strcmp(arg[0], "importpath")) {
            printf("Uso: importpath\n");
            printf("  Importa los directorios de la variable de entorno PATH del sistema al PATH local.\n");
        } else if (!strcmp(arg[0], "where")) {
            printf("Uso: where [ejecutable]\n");
            printf("  Muestra la ruta completa del archivo ejecutable buscándolo dentro del PATH.\n");
        } else if (!strcmp(arg[0], "sysinfo")) {
            printf("Uso: sysinfo\n");
            printf("  Muestra información del sistema operativo y de la máquina (uname).\n");
        } else if (!strcmp(arg[0], "open")) {
            printf("Uso: open [fichero] [modos]\n");
            printf("  Abre un fichero y añade su descriptor a la lista de ficheros abiertos del shell.\n");
            printf("  Modos posibles:\n");
            printf("    cr: O_CREAT  | ap: O_APPEND | ex: O_EXCL\n");
            printf("    ro: O_RDONLY | rw: O_RDWR   | wo: O_WRONLY | tr: O_TRUNC\n");
        } else if (!strcmp(arg[0], "close")) {
            printf("Uso: close [descriptor]\n");
            printf("  Cierra el descriptor de fichero especificado y lo elimina de la lista del shell.\n");
        } else if (!strcmp(arg[0], "listopen")) {
            printf("Uso: listopen\n");
            printf("  Muestra la lista de todos los ficheros y descriptores actualmente abiertos por el shell.\n");
        } else if (!strcmp(arg[0], "fin") || !strcmp(arg[0], "exit") ||
                   !strcmp(arg[0], "quit") || !strcmp(arg[0], "bye")) {
            printf("Uso: %s\n", arg[0]);
            printf("  Finaliza la ejecución del shell.\n");
        } else if (!strcmp(arg[0], "help")) {
            printf("Uso: help [comando]\n");
            printf("  Muestra ayuda sobre los comandos disponibles o ayuda detallada sobre un comando específico.\n");
        } else {
            printf("No hay información de ayuda detallada para '%s'\n", arg[0]);
        }
    }
}

/*
 * COMANDO: dup
 * SIRVE PARA: Duplicar un descriptor de fichero abierto (como hacer una copia del 'puntero' al archivo).

 * - dup(df) es una llamada al sistema que duplica un descriptor devolviendo el número más bajo disponible.
 * - Le metemos un control antes con 'arg[0] == NULL' porque si el usuario no pone el descriptor,
 *   en vez de petar muestra los abiertos llamando a Cmd_listopen.
 */
/* Duplica un descriptor de fichero abierto */
void Cmd_dup (char *arg[])
{
    int df, nuevo_df;
    char aux[MAXNOMBRE];

    if (arg[0] == NULL) {
        Cmd_listopen(NULL); // Si no le pasamos nada lista los abiertos
        return;
    }

    df = atoi(arg[0]); // Ahora es seguro porque sabemos que arg[0] no es NULL
    nuevo_df = dup(df); // Llamada al SO para duplicar el descriptor

    if (nuevo_df == -1) {
        perror("Imposible duplicar"); // Imprime el error exacto de errno si falla
    } else {
        // En vez de liarnos con fcntl, le ponemos un texto simple para el nombre
        sprintf(aux, "dup %d", df);
        AnadirFicheroAbierto(nuevo_df, aux, O_RDWR); // Le metemos un modo por defecto
        printf("Descriptor %d duplicado en %d\n", df, nuevo_df);
    }
}

/*
 * COMANDO: lseek
 * SIRVE PARA: Mover el cursor/puntero de lectura o escritura dentro de un archivo abierto.

 * - lseek recibe 3 cosas: descriptor (df), cuántos bytes moverlo (pos) y desde dónde medir (ref).
 * - SEEK_SET: Empieza a contar desde el PRINCIPIO del fichero.
 * - SEEK_CUR: Empieza a contar desde la POSICIÓN ACTUAL del cursor.
 * - SEEK_END: Empieza a contar desde el FINAL del fichero (sirve p.ej. para ficheros dispersos/sparse).
 */
/* Mueve el puntero/offset del fichero */
void Cmd_lseek (char *arg[])
{
    int df, ref;
    off_t pos, resultado;

    // Comprobamos que nos pasen todos los argumentos para no comer nos un cacheno de memoria vacia
    if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL) {
        printf("Uso: lseek df pos [SEEK_SET|SEEK_CUR|SEEK_END]\n");
        return;
    }

    df = atoi(arg[0]); // atoi pasa el texto que escribe el usuario en la terminal (ej: "3") (string) a numero int (3)
    pos = (off_t) atol(arg[1]); // atol pasa el texto a un número largo ('long' o 'off_t') por si el offset es grande

    // Comprobamos cuál de los 3 modos de referencia quiere el usuario
    if (!strcmp(arg[2], "SEEK_SET")) ref = SEEK_SET;      // Desde el inicio (byte 0)
    else if (!strcmp(arg[2], "SEEK_CUR")) ref = SEEK_CUR; // Desde donde estemos ahora
    else if (!strcmp(arg[2], "SEEK_END")) ref = SEEK_END; // Desde el final
    else {
        printf("Referencia no valida\n");
        return;
    }

    // lseek hace el movimiento en el kernel y nos devuelve la nueva posición absoluta en el fichero
    resultado = lseek(df, pos, ref);
    if (resultado == -1) {
        perror("Imposible realizar lseek");
    } else {
        printf("Nueva posicion del descriptor %d: %ld\n", df, (long)resultado);
    }
}

/*
 * COMANDO: readstr
 * SIRVE PARA: Leer N bytes de un fichero abierto e imprimirlos por pantalla como si fueran texto.

 * - Se usa la llamada al sistema read(df, buffer, cant).
 * - Le ponemos manualmente el caracter '\0' (nulo) al final de los bytes leídos en el array para que
 *   printf sepa dónde termina la cadena y no imprima basura o letras raras de la memoria.
 */
/* Lee N bytes de un descriptor y los muestra en pantalla */
void Cmd_readstr (char *arg[])
{
    int df, cant;
    ssize_t leidos;
    char buffer[MAXNOMBRE]; // En vez de malloc/free usamos un array local estatico y nos quitamos de memoria dinamica[cite: 5]

    if (arg[0] == NULL || arg[1] == NULL) {
        printf("Uso: readstr df cont\n");
        return;
    }

    df = atoi(arg[0]);  // Convertimos el descriptor de texto a entero
    cant = atoi(arg[1]); // Convertimos la cantidad de bytes a leer a entero

    // Si nos piden leer más de lo que cabe en nuestro array buffer, lo limitamos
    if (cant >= MAXNOMBRE) {
        cant = MAXNOMBRE - 1; // Para no salirnos del buffer
    }

    // read lee hasta 'cant' bytes del archivo asociado a 'df' y los guarda en 'buffer'
    leidos = read(df, buffer, cant);
    if (leidos == -1) {
        perror("Imposible leer");
    } else {
        buffer[leidos] = '\0'; // Nulo al final para que printf no imprima basura despues del string
        printf("%s\n", buffer);
    }
}

/*
 * COMANDO: writestr
 * SIRVE PARA: Escribir un texto dado directamente en el fichero abierto indicado.

 * - Se usa la llamada write(df, texto, tamaño).
 * - strlen(arg[1]) nos calcula la longitud exacta del texto que queremos escribir.
 */
/* Escribe una cadena en el fichero del descriptor */
void Cmd_writestr (char *arg[])
{
    int df;
    ssize_t escritos;

    if (arg[0] == NULL || arg[1] == NULL) {
        printf("Uso: writestr df cadena\n");
        return;
    }

    df = atoi(arg[0]);
    // write escribe en el archivo del descriptor 'df' la cadena pasada en arg[1]
    escritos = write(df, arg[1], strlen(arg[1]));

    if (escritos == -1) {
        perror("Imposible escribir");
    } else {
        printf("Escritos %ld bytes en el descriptor %d\n", (long)escritos, df);
    }
}

/*
 * COMANDO: makefile
 * SIRVE PARA: Crear un fichero totalmente vacío en el directorio actual.

 * - Usa open con tres banderas (flags):
 *   * O_CREAT: Crea el fichero si no existe.
 *   * O_WRONLY: Lo abre en modo solo escritura.
 *   * O_EXCL: Hace que la llamada FALLE si el fichero YA existe (así no sobrescribimos nada por error).
 * - 0666 son los permisos UNIX de creación (lectura y escritura para todos, antes de aplicar la umask).
 * - Nada más crearlo, hacemos close(df) porque solo nos pedían crearlo, no dejarlo abierto.
 */
/* Crea un archivo en blanco en el directorio */
void Cmd_makefile(char *arg[])
{
    int df;

    if (arg[0] == NULL) {
        printf("Uso: makefile nombre\n");
        return;
    }

    // O_CREAT y O_EXCL para que falle si ya existe
    df = open(arg[0], O_CREAT | O_WRONLY | O_EXCL, 0666);
    if (df == -1) {
        perror("Imposible crear el fichero");
    } else {
        close(df); // Lo creamos y cerramos al instante
    }
}

/*
 * COMANDO: makedir
 * SIRVE PARA: Crear una carpeta o directorio nuevo vacio.

 * - Usa la llamada al sistema mkdir(nombre, permisos).
 */
/* Crea una carpeta vacía */
void Cmd_makedir(char *arg[])
{
    if (arg[0] == NULL) {
        printf("Uso: makedir nombre\n");
        return;
    }

    // Le metemos todos los permisos usando las constantes de <sys/stat.h>
    if (mkdir(arg[0], S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("Imposible crear el directorio");
    }
}

/*
 * COMANDO: delete
 * SIRVE PARA: Borrar ficheros o directorios vacíos que le pasemos por parámetro.

 * - Primero usamos lstat para ver si el objeto existe y de qué tipo es (fichero o carpeta).
 * - Usamos S_ISDIR(st.st_mode) para comprobar si el modo del archivo nos dice que es directorio.
 * - Si ES directorio -> Llamamos a rmdir(nombre). Solo funciona si la carpeta está VACÍA.
 * - Si ES fichero/enlace -> Llamamos a unlink(nombre) (que borra el enlace al archivo en el sistema de ficheros).
 * - Va procesando con un bucle for todos los argumentos que le pasemos en una sola línea (delete a.txt b.txt c).
 */
/* Borra ficheros o directorios vacíos */
void Cmd_delete(char *arg[])
{
    int i;
    struct stat st; // Estructura de C donde lstat volcará la información del archivo

    if (arg[0] == NULL) {
        printf("Uso: delete nombre1 [nombre2...]\n");
        return;
    }

    // Recorremos todos los nombres que el usuario haya escrito detrás del comando
    for (i = 0; arg[i] != NULL; i++) {
        // lstat guarda en la estructura 'st' la informacion del fichero o directorio (tamano, tipo, etc.)
        if (lstat(arg[i], &st) == -1) {
            perror("Error al obtener la informacion del archivo");
            continue; // Si falla con este archivo pasa al siguiente del bucle
        }

        // Si es directorio probamos rmdir, si no unlink (borra fichero/link)
        if (S_ISDIR(st.st_mode)) { // S_ISDIR comprueba la máscara de bits del modo en la estructura stat
            if (rmdir(arg[i]) == -1) {
                perror("Imposible borrar directorio (quizas no esta vacio)");
            }
        } else {
            if (unlink(arg[i]) == -1) {
                perror("Imposible borrar archivo");
            }
        }
    }
}
