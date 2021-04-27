#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define POS_COMANDO 1
#define LUGAR_COMANDO 1
#define LUGAR_NULL_FINAL 1
#define ERROR -1
#define MAX_LINEA 512
#ifndef NARGS
#define NARGS 4
#endif

#define LARGO_ARGUMENTOS (NARGS + LUGAR_COMANDO + LUGAR_NULL_FINAL)

void ejecutar_comando(const char* comando, char* argumentos[]) {
    int resultado = fork();
    if (resultado < 0){
        fprintf(stderr,"Fallo en el fork. PID: %d - errno: %s\n",getpid(),strerror(errno));
    }
    else if(resultado == 0){
        execvp(comando,  argumentos);
        fprintf(stderr,"Ocurrio un error ejecutando el comando enviado. PID: %d - errno: %s\n",getpid(),strerror(errno));
        exit(ERROR);
    }
    else{
        wait(NULL);
    }
}

void limpiar_argumentos(char* argumentos[]){
    for (int i = 1; i < NARGS + LUGAR_COMANDO; i++) {
        free(argumentos[i]);
        argumentos[i] = NULL;
    }
}

void ejecutar_xargs(char *argv[]) {
    char linea[MAX_LINEA] = {};
    char* argumentos[LARGO_ARGUMENTOS] = {NULL};
    int argumentos_leidos = 0;

    argumentos[0] = argv[POS_COMANDO];
    
    int bytes_leidos = fscanf(stdin,"%[^\n]\n",linea);
    while(bytes_leidos > 0){
        argumentos[argumentos_leidos + LUGAR_COMANDO] = strdup(linea);
        if(argumentos[argumentos_leidos] == NULL){
            fprintf(stderr,"Ocurrio un error alocando memoria para el argumento: %s - errno: %s\n",linea,strerror(errno));
        }
        else{
            argumentos_leidos++;
        }

        if(argumentos_leidos == NARGS){
            ejecutar_comando(argv[POS_COMANDO], argumentos);
            limpiar_argumentos(argumentos);
            argumentos_leidos = 0;
        }
        bytes_leidos = fscanf(stdin,"%[^\n]\n",linea);
    }
    if(argumentos_leidos > 0){
        ejecutar_comando(argv[POS_COMANDO], argumentos);
        limpiar_argumentos(argumentos);
    }
}

int main(int argc, char* argv[]){

    if(argc == 1){
        fprintf(stderr,"No se enviaron comandos. \n");
        return ERROR;
    }

    if(isatty(0)){
        fprintf(stderr,"Los argumentos deben ser enviados a traves de un pipe. (|) \n");
        return ERROR;
    }

    ejecutar_xargs(argv);

    return 0;
}