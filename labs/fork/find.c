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

int ver_subdirectorios(char* (*funcion_busqueda)(const char*, const char*),const char *texto_a_buscar,
                       const char* nombre_directorio, int fd_directorio_padre,const char direccion_padre[PATH_MAX]);

bool puedo_ir_a_directorio(const char* nombre_directorio){
    return strcmp(nombre_directorio,DIRECTORIO_ACTUAL)!=0 && strcmp(nombre_directorio,DIRECTORIO_PADRE)!=0;
}

void imprimir_nombre(const char* direccion,const char* nombre_directorio){
    printf("%s%s\n",direccion,nombre_directorio);
    fflush(stdout);
}

void armar_direccion(const char *direccion_padre, const char *nombre_directorio, char *direccion_completa) {
    strcat(direccion_completa, direccion_padre);
    strcat(direccion_completa, nombre_directorio);
    strcat(direccion_completa, DIVISOR_DIRECTORIOS);
}

void cerrar_directorio(DIR *directorio) {
    int resultado = closedir(directorio);
    if(resultado < 0){
        fprintf(stderr,"Ocurrio un error cerrando un directorio. PID: %d - errno: %s\n",getpid(),strerror(errno));
    }
}

void iterar_directorio(char *(*funcion_busqueda)(const char *, const char *), const char *texto_a_buscar,
                       const char direccion_padre[PATH_MAX], DIR *directorio) {

    struct dirent* entrada;
    entrada = readdir(directorio);
    while(entrada != NULL){

        if(entrada->d_type == DT_REG && funcion_busqueda(entrada->d_name,texto_a_buscar)!=NULL){
            imprimir_nombre(direccion_padre,entrada->d_name);
        }
        else if(entrada->d_type == DT_DIR && puedo_ir_a_directorio(entrada->d_name)){
            if(funcion_busqueda(entrada->d_name,texto_a_buscar)!=NULL){
                imprimir_nombre(direccion_padre,entrada->d_name);
            }
            int fd_directorio = dirfd(directorio);
            if(fd_directorio < 0){
                fprintf(stderr,"Ocurrio un error convirtiendo un directorio a un fd. PID: %d - errno: %s\n",getpid(),strerror(errno));
            }
            else{
                char direccion_nueva[PATH_MAX] = {};
                armar_direccion(direccion_padre, entrada->d_name, direccion_nueva);
                ver_subdirectorios(funcion_busqueda,texto_a_buscar,entrada->d_name,fd_directorio,direccion_nueva);
            }
        }
        entrada = readdir(directorio);
    }
}

int ver_subdirectorios(char* (*funcion_busqueda)(const char*, const char*), const char *texto_a_buscar,
                       const char* nombre_directorio, const int fd_directorio_padre, const char direccion_padre[PATH_MAX]){

    int fd_directorio_actual = openat(fd_directorio_padre,nombre_directorio,O_DIRECTORY);
    if(fd_directorio_actual < 0){
        fprintf(stderr,"Ocurrio un error abriendo un fd de un directorio. PID: %d - errno: %s\n",getpid(),strerror(errno));
        return ERROR;
    }

    DIR* directorio = fdopendir(fd_directorio_actual);
    if(directorio == NULL){
        fprintf(stderr,"Ocurrio un error convirtiendo un fd a un directorio. PID: %d - errno: %s\n",getpid(),strerror(errno));
        return ERROR;
    }

    iterar_directorio(funcion_busqueda, texto_a_buscar, direccion_padre, directorio);
    cerrar_directorio(directorio);

    return 0;
}

int buscar_texto(char* (*funcion_busqueda)(const char*, const char*), const char *texto_a_buscar) {
    DIR* directorio = opendir(DIRECTORIO_ACTUAL);
    if(directorio == NULL){
        fprintf(stderr,"Ocurrio un error abriendo un directorio. PID: %d - errno: %s\n",getpid(),strerror(errno));
        return ERROR;
    }

    iterar_directorio(funcion_busqueda,texto_a_buscar,"",directorio);
    cerrar_directorio(directorio);

    return 0;
}


int main(int argc, char* argv[]){
    if(argc == 1){
        fprintf(stderr,"No se envio el texto a buscar. Ayuda: ./find texto_a_buscar\n");
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
            fprintf(stderr,"Se procede buscando %s\n",argv[POS_SENSIBILIDAD]);
        }
    }

    if(argc > 3){
        fprintf(stderr,"Se enviaron argumentos extra no reconocidos, se buscara el primer texto solamente.");
        fprintf(stderr,"Ayuda: ./find [-i] texto_a_buscar\n");
    }
    return buscar_texto(sensible_mayus ? strstr:strcasestr,argv[pos_texto]);
}