#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define LECTURA 0
#define ESCRITURA 1

#define ERROR -1
#define POSICION_NUMERO 1

void revisar_bytes(const int bytes) {
    if (bytes!=0 && bytes != sizeof(int)){
        fprintf(stderr,"No se envio/leyo la cantidad correcta de bytes. Se esperaban %lu, se obtuvieron %d\n",sizeof(int),bytes);
        fprintf(stderr,"PID: %d - errno: %s\n",getpid(),strerror(errno));
    }
}

/* PRE y POST CONDICIONES
 * El file descriptor de escritura es valido.
 */
int escribir(const int fd_escritura,const int valor){
    int bytes_escritos = write(fd_escritura,&valor,sizeof(int));
    revisar_bytes(bytes_escritos);
    return bytes_escritos;
}

/* PRE y POST CONDICIONES
 * El file descriptor de lectura es valido.
 */
int leer(const int fd_lectura, int* valor){
    int bytes_leidos = read(fd_lectura,valor,sizeof(int));
    revisar_bytes(bytes_leidos);
    return bytes_leidos;
}

void mostrar_primo(const int primo){
    printf("primo %d\n",primo);
    fflush(stdout);
}

int obtener_numero_final(const int cantidad_argumentos,char* argumentos[]){
    if (cantidad_argumentos == 1){
        fprintf(stderr,"No se envio un numero.\n");
        return ERROR;
    }
    return atoi(argumentos[POSICION_NUMERO]);
}

int crear_fork(){
    int resultado = fork();
    if(resultado < 0){
        fprintf(stderr,"Ocurrio un error al realizar un fork. PID: %d - errno: %s\n",getpid(),strerror(errno));
    }
    return resultado;
}

int crear_pipe(int fd_pipe[2]){
    int resultado = pipe(fd_pipe);
    if(resultado < 0){
        fprintf(stderr,"Ocurrio un error creando un pipe. PID: %d - errno: %s\n",getpid(),strerror(errno));
    }
    return resultado;
}

void esperar_proceso() {
    int resultado = wait(NULL);
    if(resultado < 0){
        fprintf(stderr,"Ocurrio un error esperando a un proceso. PID: %d - errno: %s\n",getpid(),strerror(errno));
    }
}

/* PRE y POST CONDICIONES
 * El file descriptor de lectura viene valido.
 */
int obtener_primo(const int fd_lectura){
    int primo;
    leer(fd_lectura, &primo);
    mostrar_primo(primo);
    return primo;
}

void cerrar_fd(const int fd){
  int resultado = close(fd);
  if(resultado < 0){
      fprintf(stderr,"Ocurrio un error cerrando un fd. PID: %d - errno: %s\n",getpid(),strerror(errno));
  }
}

bool puedo_mandar_al_siguiente_filtro(const bool hay_error,const bool hay_filtro_nuevo,const bool termino_ciclo) {
    return !hay_error && hay_filtro_nuevo && !termino_ciclo;
}

bool se_puede_leer(const int fd_lectura,int* numero,const bool hay_error,const bool termino_ciclo){
  if(termino_ciclo || hay_error){
    return false;
  }
  int bytes_leidos = leer(fd_lectura,numero);
  return bytes_leidos > 0;
}

/* PRE y POST CONDICIONES
 * Recibe los file descriptors del nodo(proceso) izquierdo en estado valido.
 */
int filtrar_numeros(const int fd_izquierdo[2]){
    cerrar_fd(fd_izquierdo[ESCRITURA]);

    int fd_derecha[2];
    int resultado = crear_pipe(fd_derecha);
    if(resultado < 0){
        cerrar_fd(fd_izquierdo[LECTURA]);
        return resultado;
    }

    bool hay_error = false, hay_filtro_nuevo = false, termino_ciclo = false;
    bool es_padre = false;
    int numero;
    int primo = obtener_primo(fd_izquierdo[LECTURA]);
    while(se_puede_leer(fd_izquierdo[LECTURA],&numero,hay_error,termino_ciclo)){
        if(numero % primo != 0){
            if(!hay_filtro_nuevo){
                resultado = crear_fork();
                if (resultado < 0) {
                    hay_error = true;
                }
                else if (resultado == 0) {
                    cerrar_fd(fd_izquierdo[LECTURA]);
                    hay_filtro_nuevo = true;
                    termino_ciclo = true;
                    filtrar_numeros(fd_derecha);
                }
                else{
                    cerrar_fd(fd_derecha[LECTURA]);
                    hay_filtro_nuevo = true;
                    es_padre = true;
                }
            }
            if(puedo_mandar_al_siguiente_filtro(hay_error, hay_filtro_nuevo, termino_ciclo)){
                escribir(fd_derecha[ESCRITURA],numero);
            }
        }
    }

    if(!hay_filtro_nuevo){
        cerrar_fd(fd_derecha[LECTURA]);
        cerrar_fd(fd_derecha[ESCRITURA]);
        cerrar_fd(fd_izquierdo[LECTURA]);
    }
    else if(es_padre){
        cerrar_fd(fd_derecha[ESCRITURA]);
        cerrar_fd(fd_izquierdo[LECTURA]);

        esperar_proceso();
    }

    return 0;
}

/* PRE y POST CONDICIONES
 * Necesita de file descriptors validos para el pipe y de un valor final valido.
 * Cumpliendo esto, generara la serie de numeros entera hasta el valor indicado.
 */
int generar_numeros(const int fd_derecho[2],const int numero_final){
    cerrar_fd(fd_derecho[LECTURA]);

    int i = 2;
    while(i<=numero_final){
        escribir(fd_derecho[ESCRITURA],i);
        i++;
    }
    cerrar_fd(fd_derecho[ESCRITURA]);

    esperar_proceso();

    return 0;
}

/* PRE y POST CONDICIONES
 * Realizara el primer paso para empezar el programa. El numero final viene ya validado.
 */
int iniciar_cadena(const int numero_final){

    int fd_pipe[2];
    int resultado = crear_pipe(fd_pipe);
    if(resultado < 0){
        return resultado;
    }

    resultado = crear_fork();
    if(resultado < 0){
        return resultado;
    }

    if(resultado == 0){
        resultado = filtrar_numeros(fd_pipe);
    }else{
        resultado = generar_numeros(fd_pipe,numero_final);
    }

    return resultado;
}

int main(int argc,char* argv[]) {
    int numero_final = obtener_numero_final(argc,argv);
    if(numero_final < 2){
        fprintf(stderr, "No se envio un numero valido.\n");
        return ERROR;
    }

    return iniciar_cadena(numero_final);
}
