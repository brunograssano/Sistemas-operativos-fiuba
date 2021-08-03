# Enunciado
## stat0

*stat* muestra en pantalla los metadatos de un archivo, incluyendo información sobre tipo de archivo,
fechas de creación y modificación, permisos, etc.
```
$ stat README.md
File: README.md
Size: 1318        Blocks: 8          IO Block: 4096   regular file
Device: 806h/2054d  Inode: 2753812     Links: 1
Access: (0644/-rw-r--r--)  Uid: ( 1000/    juan)   Gid: ( 1000/    juan)
Access: 2018-03-14 17:36:37.497432618 -0300
Modify: 2018-03-08 23:27:15.765147109 -0300
Change: 2018-03-08 23:27:15.765147109 -0300
Birth: -
```
La implementación de *stat0* mostrará únicamente el nombre, tipo y tamaño
del archivo (en bytes).

```
$ ./stat0 README.md
Size: 1318
File: README.md
Type: regular file
```

Se pide: Implementar stat0 que muestra el nombre,
tipo y tamaño en bytes de un archivo regular o directorio.

Pre-condición: el archivo existe, y es un directorio o un archivo regular.

Syscalls recomendadas: `stat(2)`. Se puede consultar también la página
de manual `inode(7)`.

# Resolución

* Solamente hay que llamar a `stat(2)` y 
  ya se tiene toda la información necesaria.
* Para el tipo de archivo, esta codificado con diferentes bits. Se utiliza una macro para
  averiguar si es un archivo regular.

Se puede compilar con:

```
gcc stat.c -o stat -std=c11 -Wall -Wextra -g 
```