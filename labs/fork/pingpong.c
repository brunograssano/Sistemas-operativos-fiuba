#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define LECTURA 0
#define ESCRITURA 1

#define RANGO_VALORES 4

void revisar_bytes(const int bytes);
void escribir(const int fd_escritura,const int valor);
void leer(const int fd_lectura, int* valor);
void mostrar_mensajes_iniciales(const int fd_padre[2],const int fd_hijo[2]);
void imprimir_mensaje_padre_inicial(const int fd_escritura,const int valor_envio,const int resultado_fork);
void imprimir_mensajes_hijo(const int fd_lectura,const int fd_escritura,const int valor);
void imprimir_mensaje_final(const int fd_lectura,const int valor,const int resultado_fork);
void cerrar_fd(const int fd);
int obtener_pipes(int fd_padre[2], int fd_hijo[2]);
void cerrar_canales(const int fd_lectura,const int fd_escritura);
int ejecutar_hijo(const int fd_padre[2],const int fd_hijo[2]);
int ejecutar_padre(const int fd_padre[2],const int fd_hijo[2],const int resultado_fork);
int enviar_mensajes(const int fd_padre[2],const int fd_hijo[2]);
int enviar_mensajes(const int fd_padre[2],const int fd_hijo[2]);

void revisar_bytes(const int bytes) {
    if (bytes != sizeof(int)){
        fprintf(stderr,"No se envio/leyo la cantidad correcta de bytes. Se esperaban %lu, se obtuvieron %d\n",sizeof(int),bytes);
    }
}

/* PRE y POST CONDICIONES
 * El file descriptor de escritura es valido.
 */
void escribir(const int fd_escritura,const int valor){
    int bytes = write(fd_escritura,&valor,sizeof(int));
    revisar_bytes(bytes);
}

/* PRE y POST CONDICIONES
 * El file descriptor de lectura es valido.
 */
void leer(const int fd_lectura, int* valor){
    int bytes = read(fd_lectura,valor,sizeof(int));
    revisar_bytes(bytes);
}

void mostrar_mensajes_iniciales(const int fd_padre[2],const int fd_hijo[2]){
    printf("Hola, soy PID %d:\n",getpid());
    printf(" - pipe me devuelve: [%d, %d]\n",fd_padre[LECTURA],fd_padre[ESCRITURA]);
    printf(" - pipe me devuelve: [%d, %d]\n",fd_hijo[LECTURA],fd_hijo[ESCRITURA]);
    fflush(stdout);
}

void imprimir_mensaje_padre_inicial(const int fd_escritura,const int valor_envio,const int resultado_fork){
    printf("Donde fork me devuelve %d\n",resultado_fork);
    printf(" - getpid me devuelve: %d\n",getpid());
    printf(" - getppid me devuelve: %d\n",getppid());
    printf(" - random me devuelve: %d\n",valor_envio);
    printf(" - envio valor %d a través de fd=%d\n",valor_envio,fd_escritura);
    fflush(stdout);
}

void imprimir_mensajes_hijo(const int fd_lectura,const int fd_escritura,const int valor){
    printf("Donde fork me devuelve 0:\n");
    printf(" - getpid me devuelve: %d\n",getpid());
    printf(" - getppid me devuelve: %d\n",getppid());
    printf(" - recibo valor %d vía fd=%d\n",valor,fd_lectura);
    printf(" - reenvío valor en fd=%d y termino\n",fd_escritura);
    fflush(stdout);
}

void imprimir_mensaje_final(const int fd_lectura,const int valor,const int resultado_fork){
    printf("Hola, de nuevo PID %d:\n",resultado_fork);
    printf(" - recibí valor %d vía fd=%d\n",valor,fd_lectura);
    fflush(stdout);
}

void cerrar_fd(const int fd){
    int resultado = close(fd);
    if(resultado < 0){
        fprintf(stderr,"Ocurrio un error cerrando un fd. PID: %d - errno: %s\n",getpid(),strerror(errno));
    }
}

int obtener_pipes(int fd_padre[2], int fd_hijo[2]){
    int resultado = pipe(fd_padre);
    if(resultado < 0){
        fprintf(stderr,"Ha ocurrido un error creando el pipe del padre. errno: %s\n",strerror(errno));
        return resultado;
    }
    resultado = pipe(fd_hijo);
    if(resultado < 0){
        fprintf(stderr,"Ha ocurrido un error creando el pipe del hijo. errno: %s\n",strerror(errno));
        cerrar_fd(fd_padre[LECTURA]);
        cerrar_fd((fd_padre[ESCRITURA]));
    }
    return resultado;
}

void cerrar_canales(const int fd_lectura,const int fd_escritura){
    cerrar_fd(fd_lectura);
    cerrar_fd(fd_escritura);
}

/* PRE y POST CONDICIONES
 * Ambos pares de file descriptors vienen validos.
 */
int ejecutar_hijo(const int fd_padre[2],const int fd_hijo[2]){
    cerrar_canales(fd_hijo[LECTURA],fd_padre[ESCRITURA]);

    int valor;
    leer(fd_padre[LECTURA],&valor);
    imprimir_mensajes_hijo(fd_padre[LECTURA], fd_hijo[ESCRITURA],valor);
    escribir(fd_hijo[ESCRITURA],valor);

    cerrar_canales(fd_padre[LECTURA],fd_hijo[ESCRITURA]);
    return 0;
}

/* PRE y POST CONDICIONES
 * Ambos pares de file descriptors vienen validos.
 */
int ejecutar_padre(const int fd_padre[2],const int fd_hijo[2],const int resultado_fork){
    cerrar_canales(fd_padre[LECTURA],fd_hijo[ESCRITURA]);

    int valor = random() % RANGO_VALORES;
    imprimir_mensaje_padre_inicial(fd_padre[ESCRITURA],valor,resultado_fork);

    escribir(fd_padre[ESCRITURA],valor);
    leer(fd_hijo[LECTURA],&valor);

    imprimir_mensaje_final(fd_hijo[LECTURA],valor,resultado_fork);
    cerrar_canales(fd_hijo[LECTURA],fd_padre[ESCRITURA]);
    return 0;
}

/* PRE y POST CONDICIONES
 * Ambos pares de file descriptors vienen validos.
 */
int enviar_mensajes(const int fd_padre[2],const int fd_hijo[2]) {

    mostrar_mensajes_iniciales(fd_padre,fd_hijo);
    int resultado = fork();
    if (resultado < 0){
        fprintf(stderr,"Ha ocurrido un error creando el fork. errno: %s\n",strerror(errno));
        return resultado;
    }

    if(resultado == 0){
        resultado = ejecutar_hijo(fd_padre,fd_hijo);
    }
    else{
        resultado = ejecutar_padre(fd_padre,fd_hijo,resultado);
    }
    return resultado;
}

int main() {
    srandom(time(NULL));
    int fd_padre[2],fd_hijo[2];

    int resultado = obtener_pipes(fd_padre, fd_hijo);
    if (resultado < 0){
        fprintf(stderr,"Ha ocurrido un error creando los pipes correspondientes.\n");
        return resultado;
    }

    return enviar_mensajes(fd_padre,fd_hijo);
}
