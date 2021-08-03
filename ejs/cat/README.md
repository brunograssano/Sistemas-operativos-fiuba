# Enunciado
## cat0

*cat (concatenate)* es una herramienta unix que permite concatenar archivos y mostrarlos por salida estándar.
En este lab se implementará una vesión simplificada de *cat*, que muestra en pantalla los contenidos de un único archivo.

```
$ cat ejemplo.txt
Sistemas Operativos, 1er cuatrimestre 2018
```

Se pide: Implementar *cat0* que toma un archivo regular y muestra su contenido por salida estándar.

Pre-condición: solo se pasa un archivo, este archivo existe y se tienen permisos de lectura.

Syscalls recomendadas: open, read, write, close.


# Resolución

* La resolución es bastante sencilla también, hay que utilizar las 4 *syscalls* en el orden dado.
* Hay que tener en cuenta que tanto `read(2)` como `write(2)` pueden
  leer/escribir menos que lo indicado.
* La implementación acepta varios argumentos.



Se puede compilar con:

```
gcc cat.c -o cat -std=c11 -Wall -Wextra -g 
```