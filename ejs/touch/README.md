# Enunciado
## touch0

*touch* toma como parámetro un archivo (de cualquier tipo) y permite actualizar su metadata,
especialmente las fechas de último acceso (atime) y última modificación (mtime);
ambos atributos pueden verse mediante el comando stat.
Una llamada a touch sobre un archivo actualiza ambas fechas al tiempo actual.

No obstante, el uso más común del comando touch es la creación de archivos regulares:
si el parámetro referencia a un archivo que no existe, se lo crea.
La primera versión touch0 sólo implementará esta funcionalidad de touch.

Se pide: Implementar touch0 que toma como parámetro un archivo y
lo crea en caso de que no exista (el archivo creado debe estar en blanco).
Si el archivo ya existía, no se hace nada.

Ejemplo:
```
$ ls
touch0
$ ./touch0 un_archivo
$ ls
touch0 un_archivo
$ stat un_archivo
File: un_archivo
Size: 0     Blocks: 0    IO Block: 4096   regular empty file
[...]
```

Notar que el tamaño del archivo creado es 0,
y stat también nos lo indica enunciando regular empty file.

Pre-condición: si el archivo existe, es un archivo regular.

Syscalls recomendadas: open.

## touch1

Revisitar la implementación de *touch0* realizada en los puntos anteriores y
agregarle la funcionalidad de actualización de las fechas en la metadata de un
archivo en caso de que ya exista. Tal metadata puede verse mediante el comando *stat*.

Un ejemplo del uso de *touch1* sería:
```
$ stat archivo
[...]
Access: 2018-03-16 17:31:48.722017895 -0300
Modify: 2018-03-14 17:27:56.438147960 -0300
Change: 2018-03-14 17:27:56.438147960 -0300
[...]
$ ./touch0 archivo
$ stat archivo
[...]
Access: 2018-03-21 00:58:04.671902112 -0300
Modify: 2018-03-21 00:58:04.671902112 -0300
Change: 2018-03-21 00:58:04.671902112 -0300
[...]
```

Se actualizaron todas las fechas asociadas al archivo, pero los contenidos del mismo no se vieron modificados, sólo se alteró la metadata.

Implementar *touch1* que toma un archivo como parámetro.
Si no existe, crea un archivo regular vacío.
Si el archivo existía previamente, modifica las fechas de acceso y
de modificación al tiempo actual.

Syscalls recomendadas: `utime(2)` y la función `futimes(3)`.

# Resolución

* El primer ejercicio consiste en llamar a `open(2)` con los argumentos correctos.
* En el segundo caso se llama a `utime(2)` y actualiza el
  tiempo de acceso al archivo
  (pasando NULL como segundo argumento toma el tiempo actual).
* Se permite realizar *touch* para múltiples archivos.

Se puede compilar con:

```
gcc touch.c -o touch -std=c11 -Wall -Wextra -g 
```