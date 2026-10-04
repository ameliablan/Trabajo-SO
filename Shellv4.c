/**Shellv4.c: Es el fichero principal del programa (main).
* Contiene el bucle principal (while) que lee continuamente
* las entradas de la terminal mediante fgets,
* trocea la cadena introducida por el usuario en comandos y argumentos (TrocearCadena)
* , y decide si se trata de un comando interno del shell (buscándolo en la tabla C[])
* o si debe lanzarse como un ejecutable externo en primer o segundo plano.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


#include "ejemplo.h"


#define MAXENTRADA 2048

struct COMANDO{
  char * nombre;
  void (*funcion)(char**);
};

/**************************SHELL**************************/

static struct COMANDO C[] = {
   {"fin", Cmd_fin},
   {"exit", Cmd_fin},
   {"quit", Cmd_fin},
   {"bye", Cmd_fin}, // Funcionara igual que exit
   {"date", Cmd_date},
   {"pid", Cmd_pid},
   {"pwd", Cmd_pwd},
   {"chdir", Cmd_chdir},
   {"autores", Cmd_autores},
   {"exec", Cmd_exec},
   {"pplano", Cmd_pplano},
   {"splano", Cmd_splano},
   {"path", Cmd_path},
   {"importpath", Cmd_importpath},
   {"where", Cmd_where},
   {"sysinfo", Cmd_sysinfo},
   {"open", Cmd_open},
   {"close", Cmd_close},
   {"listopen", Cmd_listopen},
   {"help", Cmd_help},
   {"dup", Cmd_dup},
   {"lseek", Cmd_lseek},
   {"readstr", Cmd_readstr},
   {"writestr", Cmd_writestr},
   {"makefile", Cmd_makefile},
   {"makedir", Cmd_makedir},
   {"delete", Cmd_delete},
   /* Incluir funciones de list y deltree al implementarlas */
   {NULL, NULL},
};

void DecidirComando (char *tr[])
{
  int i;

  if (tr[0]==NULL) return; /*superfluo, por si cambio otras cosas */
  for (i=0; C[i].nombre!=NULL; i++)
    if (!strcmp(C[i].nombre,tr[0])){
        (*C[i].funcion)(tr+1);
	    return;
    }
  Cmd_pplano(tr); /*si no es un comando, es un  ejecutable externo: en primer plano*/
}

int TrocearCadena(char * cadena, char * trozos[])
{
  int i=1;
  if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
      return 0;
  while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
     i++;
  return i;
}	
void ProcesarEntrada(char * entrada)
{
   char *tr[MAXENTRADA/2];
   if (TrocearCadena(entrada,tr)==0) /*no hay nada*/
	return;
   DecidirComando(tr);
}

int  main(int argc, char *argv[], char *ent[])
{
   char entrada[MAXENTRADA];

   if (argv[1]==NULL)
        printf ("Ejecutando con path vacio: %s -p para importar el path\n",argv[0]);
   else if (!strcmp(argv[1],"-p"))
        Cmd_importpath(NULL);

   while (1){
      printf ("-> ");
      fgets(entrada,MAXENTRADA,stdin);
      ProcesarEntrada(entrada);
   }
}
