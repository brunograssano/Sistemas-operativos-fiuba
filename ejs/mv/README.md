# Enunciado
## mv0 ☆

*mv (move)* permite mover un archivo (regular o directorio) de un directorio
a otro. El archivo no se mueve físicamente sino que sólo se renombra y 
se modifica el enlace al mismo en su directorio actual.
La implementación de *mv0* tendrá la misma funcionalidad que *mv*.

Un ejemplo del uso de *mv0* podría ser:
```
$ ls
directorio1   archivo1
$ ls directorio1
$ mv archivo1 directorio1/archivo2
$ ls
directorio1
$ ls directorio1
archivo2
```
Se pide: Implementar *mv0*, que permite mover un archivo de un directorio a otro.

Pre-condición: el archivo destino no existe.

*Syscalls* recomendadas: `rename(2)`.

Pregunta: ¿se puede usar *mv0* para renombrar archivos dentro del mismo directorio?

# Resolución

* Se llama a la *sycall* `rename(2)` con el *path* viejo y nuevo y ya está resuelto.
* Respuesta a la pregunta: Si, se puede usar para renombrar archivos dentro del mismo directorio.

Se puede compilar con:

```
gcc mv.c -o mv -std=c11 -Wall -Wextra -g 
```