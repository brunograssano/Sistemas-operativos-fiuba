#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#define ERROR -1

#define POS_SENSIBILIDAD 1
#define POS_TEXTO_DEFAULT 1
#define SIN_SENSIBILIDAD "-i"

#define DIRECTORIO_ACTUAL "."
#define DIRECTORIO_PADRE ".."
#define DIVISOR_DIRECTORIOS "/"


bool puedo_ir_a_directorio(char* nombre_directorio){
    return strcmp(nombre_directorio,DIRECTORIO_ACTUAL)!=0 && strcmp(nombre_directorio,DIRECTORIO_PADRE)!=0;
}

void imprimir_nombre(char* direccion,char* nombre_directorio){
    printf("%s%s\n",direccion,nombre_directorio);
    fflush(stdout);
}

int ver_subdirectorios(char* (*funcion_busqueda)(const char*,const char*), char *texto_a_buscar,char* nombre_directorio,int fd_directorio_padre,char direccion_padre[PATH_MAX]){
    int fd_directorio_actual = openat(fd_directorio_padre,nombre_directorio,O_DIRECTORY);
    if(fd_directorio_actual < 0){
        fprintf(stderr,"Ocurrio un error abriendo un fd de un directorio.\n");
        return ERROR;
    }

    DIR* directorio = fdopendir(fd_directorio_actual);
    if(directorio == NULL){
        fprintf(stderr,"Ocurrio un error convirtiendo un fd a un directorio.\n");
        return ERROR;
    }

    struct dirent* entrada;
    entrada = readdir(directorio);
    while(entrada != NULL){
        if(entrada->d_type == DT_REG && funcion_busqueda(entrada->d_name,texto_a_buscar)!=NULL){
            imprimir_nombre(direccion_padre,entrada->d_name);
        }
        else if(entrada->d_type == DT_DIR && puedo_ir_a_directorio(entrada->d_name)){
            char direccion[PATH_MAX] = {};
            if(funcion_busqueda(entrada->d_name,texto_a_buscar)!=NULL){
                imprimir_nombre(direccion_padre,entrada->d_name);
            }
            int fd_directorio = dirfd(directorio);
            if(fd_directorio < 0){
                fprintf(stderr,"Ocurrio un error convirtiendo un directorio a un fd.\n");
            }
            strcat(direccion,direccion_padre);
            strcat(direccion,entrada->d_name);
            strcat(direccion,DIVISOR_DIRECTORIOS);
            ver_subdirectorios(funcion_busqueda,texto_a_buscar,entrada->d_name,fd_directorio,direccion);
        }
        entrada = readdir(directorio);
    }

    int resultado = closedir(directorio);
    if(resultado < 0){
        fprintf(stderr,"Ocurrio un error cerrando un directorio.\n");
    }

    return 0;
}

int buscar_texto(char* (*funcion_busqueda)(const char*,const char*), char *texto_a_buscar) {

    DIR* directorio = opendir(DIRECTORIO_ACTUAL);
    if(directorio == NULL){
        fprintf(stderr,"Ocurrio un error abriendo un directorio.\n");
        return ERROR;
    }

    struct dirent* entrada;
    entrada = readdir(directorio);
    while(entrada != NULL){
        if(entrada->d_type == DT_REG && funcion_busqueda(entrada->d_name,texto_a_buscar)!=NULL){
            imprimir_nombre("",entrada->d_name);
        }
        else if(entrada->d_type == DT_DIR && puedo_ir_a_directorio(entrada->d_name)){
            char direccion[PATH_MAX] = {};
            if(funcion_busqueda(entrada->d_name,texto_a_buscar)!=NULL){
                imprimir_nombre("",entrada->d_name);
            }
            int fd_directorio = dirfd(directorio);
            if(fd_directorio < 0){
                fprintf(stderr,"Ocurrio un error convirtiendo un directorio a un fd.\n");
            }
            strcat(direccion,entrada->d_name);
            strcat(direccion,DIVISOR_DIRECTORIOS);
            ver_subdirectorios(funcion_busqueda,texto_a_buscar,entrada->d_name,fd_directorio,direccion);
        }
        entrada = readdir(directorio);
    }

    int resultado = closedir(directorio);
    if(resultado < 0){
        fprintf(stderr,"Ocurrio un error cerrando un directorio.\n");
    }

    return 0;
}


int main(int argc, char* argv[]){
    if(argc == 1){
        fprintf(stderr,"No se envio el texto a buscar. Ayuda: ./find tex\n");
        return ERROR;
    }

    bool sensible_mayus = true;
    int pos_texto = POS_TEXTO_DEFAULT;
    if(argc == 3){
        if(strcmp(argv[POS_SENSIBILIDAD],SIN_SENSIBILIDAD) == 0){
            sensible_mayus = false;
            pos_texto++;
        }
        else{
            fprintf(stderr,"Se esperaba %s y se obtuvo como argumento %s.\n",SIN_SENSIBILIDAD,argv[POS_SENSIBILIDAD]);
        }
    }

    if(argc > 3){
        fprintf(stderr,"Se enviaron argumentos extra no reconocidos. Ayuda: ./find tex\n");
    }
    return buscar_texto(sensible_mayus ? strstr:strcasestr,argv[pos_texto]);
}
