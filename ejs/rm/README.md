# Enunciado
## rm0

*rm (remove)* es la herramienta unix que permite eliminar archivos y directorios.

El uso estándar `rm <file>` permite borrar solo archivos regulares, y arrojará error si se intenta eliminar un directorio.

Para la implementación de rm0 solo se considerará el caso de archivos regulares.
```
$ ls
archivo1   archivo2   directorio1   rm
$ ./rm archivo1
$ ls
archivo2   directorio1   rm0
```

Se pide: implementar *rm0* que elimina un archivo regular.

Pre-condición: el archivo existe y es regular.

Syscalls recomendadas: unlink.

## rm1

Mostrar cómo se usaría `errno` y `perror(3)` para obtener el siguiente comportamiento de *rm*:
```
$ ./rm1 directorio1
rm: cannot remove 'directorio1': Is a directory
```

# Resolución

La resolución de los ejercicios es bastante sencilla,
 1. Se revisa que se tenga una cantidad adecuada de argumentos.
 2. Se agrego para poder eliminar más de un archivo, por lo que se itera la lista de argumentos
 3. Se llama a la *syscall* `unlink(2)`, la cual solamente requiere el nombre del archivo.
 4. Se imprime por pantalla el resultado.
 5. Mostrar el error es solamente revisar `errno`, se puede convertir a *string* con `strerror(3)`.

Se puede compilar con:

```
gcc rm.c -o rm -std=c11 -Wall -Wextra -g 
```