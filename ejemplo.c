/**
 * ejemplo.c: Contiene la implementación de la gran mayoría de los comandos
 * internos del shell (como chdir, pwd, pid, path, where, exec, etc.) y las
 * funciones auxiliares para la gestión de procesos (fork, waitpid) y control
 * de la ejecución en primer o segundo plano (splano, pplano).
 */

#include "ejemplo.h"

//NUEVO
/* Lista global o array para almacenar los ficheros abiertos por el shell */
#define MAX_FICH_ABIERTOS 64
static struct fich_abierto ficheros_abiertos[MAX_FICH_ABIERTOS];

void AnadirFicheroAbierto(int desc, char *nombre, int modo) {
    int i;
    for (i = 0; i < MAX_FICH_ABIERTOS; i++) {
        if (ficheros_abiertos[i].descriptor == -1) {
            ficheros_abiertos[i].descriptor = desc;
            strncpy(ficheros_abiertos[i].nombre, nombre, MAXNOMBRE - 1);
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

void InicializarFicherosAbiertos() {
    int i;
    for (i = 0; i < MAX_FICH_ABIERTOS; i++) {
        ficheros_abiertos[i].descriptor = -1;
    }
    /* Añadir por defecto la entrada estándar (0), salida estándar (1) y error estándar (2) */
    AnadirFicheroAbierto(0, "stdin", O_RDONLY);
    AnadirFicheroAbierto(1, "stdout", O_WRONLY);
    AnadirFicheroAbierto(2, "stderr", O_WRONLY);
}

void AnadirAlPath(char *dir)
{
    if (dir==NULL)
        PathPrint();
    else if (PathAdd(dir)==-1)
        perror("Imposible aniadir");
}


//ESTRUCTURA INICIAL
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

// AÑADIDO
void Cmd_autores(char *arg[])
{
  if (arg[0] == NULL) {
      printf("Autor 1: Nombre Apellido (login1)\n");
      printf("Autor 2: Nombre Apellido (login2)\n");
  } else if (!strcmp(arg[0], "-l")) {
      printf("login1\nlogin2\n");
  } else if (!strcmp(arg[0], "-n")) {
      printf("Nombre Apellido\nNombre Apellido\n");
  } else {
      printf("Uso: autores [-l | -n]\n"); //ACLARACIÓN en caso de mal uso
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

/** NUEVOS COMANDOS
*/

void Cmd_date (char * arg[])
{
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
        printf("Uso: date [-d | -t]\n"); //ACLARACIÓN
    }


void Cmd_sysinfo(char *arg[])
{
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

void Cmd_help(char *arg[])
{
    if (arg[0] == NULL) {
        printf("Comandos disponibles: fin, exit, quit, bye, date, pid, pwd, chdir, autores, exec, pplano, splano, path, importpath, where, sysinfo, help, open, close, listopen\n");
    } else {
        printf("Ayuda detallada para el comando: %s\n", arg[0]);
    }
}

void Cmd_open(char *arg[])
{
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

    /* Si no se especifica modo por defecto lectura */
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

void Cmd_close(char *arg[])
{
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

void Cmd_listopen(char *arg[])
{
    int i;
    printf("Ficheros abiertos:\n");
    for (i = 0; i < MAX_FICH_ABIERTOS; i++) {
        if (ficheros_abiertos[i].descriptor != -1) {
            printf("Descriptor: %d | Nombre: %s\n", ficheros_abiertos[i].descriptor, ficheros_abiertos[i].nombre);
        }
    }
}