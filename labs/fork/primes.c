#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define LECTURA 0
#define ESCRITURA 1

#define ERROR -1
#define POSICION_NUMERO 1

const int TERMINAR_PIPE = -1;
int preparar_filtrado_de_numeros(const int fd_izquierdo[2]);

void revisar_bytes(const int bytes) {
    if (bytes!=0 && bytes != sizeof(int)){
        fprintf(stderr,"No se envio/leyo la cantidad correcta de bytes. Se esperaban %lu, se obtuvieron %d\n",sizeof(int),bytes);
        fprintf(stderr,"PID: %d - errno: %s\n",getpid(),strerror(errno));
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

bool cerro_pipe(const int numero){
    return numero == TERMINAR_PIPE;
}

void mostrar_primo(const int primo){
    printf("primo %d\n",primo);
}

bool hay_que_crear_nuevo_filtro(const bool hay_filtro_nuevo,const int numero){
  return !hay_filtro_nuevo && !cerro_pipe(numero);
}

bool hay_que_terminar(const bool hay_error,const bool termino_ciclo){
  return hay_error || termino_ciclo;
}

bool puedo_mandar_al_siguiente_filtro(const bool hay_error,const bool hay_filtro_nuevo) {
    return !hay_error && hay_filtro_nuevo;
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
        fprintf(stderr,"Ocurrio un error creando el pipe. PID: %d - errno: %s\n",getpid(),strerror(errno));
    }
    return resultado;
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

/* PRE y POST CONDICIONES
 * Creara un nuevo filtro, el file descriptor correspondiente viene valido.
 * Dejara tambien el ciclo viejo del filtro nuevo terminado.
 */
void crear_filtro(const int fd_derecha[2], bool *hay_error, bool *hay_filtro_nuevo, bool *termino_ciclo) {
    int resultado = crear_fork();
    if (resultado < 0) {
        (*hay_error) = true;
    }
    if (resultado == 0) {
        preparar_filtrado_de_numeros(fd_derecha);
        (*termino_ciclo) = true;
    }else if(!(*hay_error)){
        close(fd_derecha[LECTURA]);
        (*hay_filtro_nuevo) = true;
    }
}

/* PRE y POST CONDICIONES
 * El file descriptor de lectura correspondiente a la izquierda y los del siguiente nodo(derecha) vienen validos.
 * Aplicara el algoritmo de la criba de Eratostenes (pensado como filtro).
 */
bool filtrar_numeros(const int fd_lectura, const int fd_derecha[2]) {
    bool hay_error = false, hay_filtro_nuevo = false, termino_ciclo = false;
    int primo = obtener_primo(fd_lectura);
    int numero;
    leer(fd_lectura, &numero);
    while(!hay_que_terminar(hay_error,termino_ciclo)){
        if(numero % primo != 0){
            if(hay_que_crear_nuevo_filtro(hay_filtro_nuevo,numero)){
                crear_filtro(fd_derecha, &hay_error, &hay_filtro_nuevo, &termino_ciclo);
            }
            if(puedo_mandar_al_siguiente_filtro(hay_error, hay_filtro_nuevo)){
                escribir(fd_derecha[ESCRITURA],numero);
            }
        }
        if(!hay_que_terminar(hay_error,termino_ciclo)){
            if(cerro_pipe(numero)){
                termino_ciclo = true;
            }
            else{
                leer(fd_lectura, &numero);
            }
        }
    }

    return hay_filtro_nuevo;
}

/* PRE y POST CONDICIONES
 * Recibe los file descriptors del nodo(proceso) izquierdo en estado valido.
 */
int preparar_filtrado_de_numeros(const int fd_izquierdo[2]){
    close(fd_izquierdo[ESCRITURA]);

    int fd_derecha[2];
    int resultado = crear_pipe(fd_derecha);
    if(resultado < 0){
        return resultado;
    }

    bool hubo_filtro_nuevo = filtrar_numeros(fd_izquierdo[LECTURA], fd_derecha);

    if(!hubo_filtro_nuevo){
        close(fd_derecha[LECTURA]);
    }
    close(fd_derecha[ESCRITURA]);
    close(fd_izquierdo[LECTURA]);

    return 0;
}

/* PRE y POST CONDICIONES
 * Necesita de un file descriptor de escritura valido.
 * Si se cumple, enviara a la serie de filtros el valor indicado,
 * esperando un tiempo para que los otros procesos lo procesen correctamente.
 */
void enviar_a_la_cadena(const int fd_escritura,const int valor){
    escribir(fd_escritura,valor);
    usleep(3);
}


/* PRE y POST CONDICIONES
 * Necesita de file descriptors validos para el pipe y de un valor final valido.
 * Cumpliendo esto, generara la serie de numeros entera hasta el valor indicado.
 */
int generar_numeros(const int fd_derecho[2],const int numero_final){
    close(fd_derecho[LECTURA]);

    int i = 2;
    while (i<=numero_final){
        enviar_a_la_cadena(fd_derecho[ESCRITURA],i);
        i++;
    }
    enviar_a_la_cadena(fd_derecho[ESCRITURA],TERMINAR_PIPE);

    close(fd_derecho[ESCRITURA]);
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
        resultado = preparar_filtrado_de_numeros(fd_pipe);
    }else{
        resultado = generar_numeros(fd_pipe,numero_final);
    }

    return resultado;
}

int main(int argc,char* argv[]) {
    int numero_final = obtener_numero_final(argc,argv);
    if (numero_final < 2){
        fprintf(stderr, "No se envio un numero valido.\n");
        return ERROR;
    }

    return iniciar_cadena(numero_final);
}