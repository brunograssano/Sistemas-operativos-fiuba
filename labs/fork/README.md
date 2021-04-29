# Lab fork

El objetivo de este lab es familiarizarse con las llamadas al sistema fork() (que crea una copia del proceso actual) y pipe() (que proporciona un mecanismo de comunicación unidireccional entre dos procesos).

## Compilación
Para la compilación de los ejercicios del lab, se puede utilizar el makefile.

```
make pingpong
make primes
make find
make xargs
make all
```

## Pingpong
### Enunciado
Se pide escribir un programa en C que use fork() y pipe() para enviar y recibir de vuelta (ping-pong) un determinado valor entero entre dos procesos. El valor se debe crear con random(3) una vez ambos procesos existan.

El programa debe imprimir por pantalla, en el formato exacto que se especifica a continuación, la secuencia de eventos en ambos procesos:
```
$ ./pingpong
Hola, soy PID <x>:
  - pipe me devuelve: [3, 4]
  - pipe me devuelve: [6, 7]

Donde fork me devuelve <y>:
  - getpid me devuelve: <?>
  - getppid me devuelve: <?>
  - random me devuelve: <v>
  - envío valor <v> a través de fd=?

Donde fork me devuelve 0:
  - getpid me devuelve: <?>
  - getppid me devuelve: <?>
  - recibo valor <v> vía fd=?
  - reenvío valor en fd=? y termino

Hola, de nuevo PID <x>:
  - recibí valor <v> vía fd=?
```

Ayuda:

* Nótese que como las tuberías —pipes— son unidireccionales, se necesitarán dos para poder transmitir el valor en una dirección y otra.
* Para obtener números aleatorios que varíen en cada ejecución del programa, se debe inicializar el PRNG (generador de números pseudo-aleatorios) mediante la función srandom() (típicamente con el valor de time()).
* Si fork() fallase, simplemente se imprime un mensaje por salida de error estándar, y el programa termina.

## Primes
### Enunciado
La criba de Eratóstenes (sieve of Eratosthenes en inglés) es un algoritmo milenario para calcular todos los primos menores a un determinado número natural, n.

Si bien la visualización del algoritmo suele hacerse “tachando” en una grilla, el concepto de criba, o sieve (literalmente: cedazo, tamiz, colador) debe hacernos pensar más en un filtro. En particular, puede pensarse en n filtros apilados, donde el primero filtra los enteros múltiplos de 2, el segundo los múltiplos de 3, el tercero los múltiplos de 5, y así sucesivamente.

Si modelásemos cada filtro como un proceso, y la transición de un filtro al siguiente mediante tuberías (pipes), se puede implementar el algoritmo con el siguiente pseudo-código (ver fuente original, y en particular la imagen que allí se muestra):
```
p := <leer valor de pipe izquierdo>

imprimir p, asumiendo que es primo

mientras <pipe izquierdo no cerrado>:
    n = <leer siguiente valor de pipe izquierdo>
    si n % p != 0:
        escribir <n> en el pipe derecho
```

(El único proceso que es distinto es el primero, que tiene que simplemente generar la secuencia de números naturales de 2 a n. No tiene lado izquierdo.)

La interfaz que se pide es:
```
$ ./primes <n>
```

Donde n será un número natural mayor o igual a 2. El código debe crear una estructura de procesos similar a la mostrada en la imagen, de tal manera que:

* el primer proceso cree un proceso derecho, con el que se comunica mediante un pipe
* ese primer proceso escribe en el pipe la secuencia de números de 2 a n, para a continuación cerrar el pipe y esperar la finalización del proceso derecho
* todos los procesos sucesivos aplican el pseudo-código mostrado anteriormente, con la salvedad de que son responsables de crear a su “hermano” derecho, y la tubería que los comunica.

Ejemplo de uso:
```
$ ./primes 35
primo 2
primo 3
primo 5
primo 7
primo 11
primo 13
primo 17
primo 19
primo 23
primo 29
primo 31
```

### Detalles de implementación
Para este ejercicio, al realizar el fork inicial, el proceso original se encargará de generar la cadena de números (desde 2 hasta el número enviado como argumento)
que será enviada de uno a uno a través del pipe. El proceso nuevo será el primer filtro.

Al comienzo de la llamada del filtrado (`int filtrar_numeros(const int fd_izquierdo[2])`) se crea el pipe de comunicación derecho con el proceso nuevo
en caso de ser creado, esto se hizo en este lugar para evitar otro posible caso de error dentro del ciclo. Si resulta que no se necesita un nuevo pipe
(solamente en el último filtro), este es cerrado junto con el fd de lectura final.

Al momento en que finaliza el envío de números, el proceso se queda esperando a que termine su proceso hijo. Lo mismo sucede con el siguiente, y así continuamente
hasta que termine el último. (Esperan en cadena). Una vez que haya terminado el último filtro empieza el desenrollado hasta el proceso original.

Del manejo de errores, se notifica al usuario a través de `stderr` en varios de los casos donde puede fallar, y dependiendo de la importancia se sigue o no.
Por ej, si falla la creación de uno de los pipes requeridos se devuelve el resultado del error y se va a ir notificando al stack del filtrado de números del proceso
hasta llegar al comienzo del programa. Lo mismo sucede en el caso del fork. De esta forma se asegura el cierre de los fd que corresponda y que no termine el proceso
de forma forzosa.

En otros casos de errores, como el fallo de `wait` o `close` solamente se notifica al usuario y se sigue con el programa, ya que esta en el final de la ejecución.

## Find
### Enunciado
Se pide escribir una versión muy simplificada de la utilidad find. La herramienta find tal y como se la encuentra en sistemas GNU/Linux acepta una miríada de opciones (ver su página de manual, o un resumen gráfico), pero en este lab se implementará una sola.

La sinopsis de nuestra implementación será:
```
$ ./find [-i] <cadena>
```
Invocado como ./find xyz, el programa buscará y mostrará por pantalla todos los archivos del directorio actual (y subdirectorios) cuyo nombre contenga (o sea igual a) xyz. Si se invoca como ./find -i xyz, se realizará la misma búsqueda, pero sin distinguir entre mayúsculas y minúsculas.

Por ejemplo, si en el directorio actual se tiene:
```
.
├── Makefile
├── find.c
├── xargs.c
├── antiguo
│   ├── find.c
│   ├── xargs.c
│   ├── pingpong.c
│   ├── basurarghh
│   │   ├── find0.c
│   │   ├── find1.c
│   │   ├── pongg.c
│   │   └── findddddddd.c
│   ├── planes.txt
│   └── pingpong2.c
├── antinoo.jpg
└── GNUmakefile
```

El resultado de las distintas invocaciones debe ser como sigue (no importa el orden en que se impriman los archivos de un mismo directorio):
```
$ find akefile
Makefile
GNUmakefile

$ find Makefile
Makefile

$ find -i Makefile
Makefile
GNUmakefile

$ find arg
xargs.c
antiguo/xargs.c
antiguo/basurarghh

$ find pong
antiguo/pingpong.c
antiguo/basurarghh/pongg.c
antiguo/pingpong2.c

$ find an
antiguo
antiguo/planes.txt
antinoo.jpg

$ find d.c
find.c
antiguo/find.c
antiguo/basurarghh/findddddddd.c
```

Ayuda:

* Usar recursión para descender a los distintos directorios.
* Nunca descender los directorios especiales `.` y `..` (son, cada uno, un “alias”; el primero al directorio actual, el segundo a su directorio inmediatamente superior).
* No es necesario preocuparse por ciclos en enlaces simbólicos.
* En el resultado de readdir(), asumir que el campo d_type siempre está presente, y es válido.
* La implementación case-sensitive vs. case-insensitive (opción `-i`) se puede resolver limpiamente usando un puntero a función como abstracción. (Ver `strstr(3)`.)

Requisitos:

* Llamar a la función `opendir()` una sola vez, al principio del programa (con argumento "."; no es necesario conseguir el nombre del directorio actual, si tenemos su alias).
* Para abrir sub-directorios, usar exclusivamente la función `openat(2)` (con el flag O_DIRECTORY como precaución). De esta manera, no es necesario realizar concatenación de cadenas para abrir subdirectorios.
* Sí será necesario, no obstante, concatenar cadenas para mostrar por pantalla. No es necesario usar memoria dinámica; es suficiente un único buffer estático de longitud `PATH_MAX`.
* Funciones que resultarán útiles como complemento a `openat()`: `dirfd()`, `fdopendir()`.


### Detalles de implementación
En este ejercicio se hace el recorrido de los subdirectorios de forma recursiva, llamando a la función `int ver_subdirectorios(...)` la cual llama a
`int iterar_directorio(...)` para realizar el ciclo de búsqueda. Si en algún momento falla la apertura de un subdirectorio debido a `openat` o `fopendir`,
se notifica y se retorna el error. Si la función anterior estaba iterando, lo que va a suceder es que se 'saltea' ese subdirectorio debido al error, por lo que el resto de
la búsqueda continuará hasta que se llegue al `EOF` de las entradas del subdirectorio abierto correctamente. Esto evita terminar el programa y no poder cerrar correctamente los directorios ya abiertos.

## Xargs
### Enunciado
Se pide escribir una versión simplificada de la utilidad estándar xargs. En su versión más sencilla, xargs:

* toma un único argumento (argv[1], un comando)
* lee nombres de archivo de entrada estándar, uno por línea
* para cada nombre de archivo leído, invoca al comando especificado con el nombre de archivo como argumento

Por ejemplo, si se invoca:
```
echo /home | xargs ls
```
Eso sería equivalente a realizar `ls /home`


Requisitos de la implementación:
* se leerán los nombres de archivo línea a línea (se recomienda usar la función `getline(3)`), nunca separados por espacios. Se deberá, eso sí, eliminar el caracter '\n' para obtener el nombre del archivo.
* el “empaquetado” vendrá definido por un valor entero positivo disponible en la macro NARGS. El programa debe funcionar de manera tal que siempre se pasen NARGS argumentos al comando ejecutado (excepto en su última ejecución, que pueden ser menos). Si no se encuentra definida `NARGS`, se debe definir a 4. (Usar, por tanto, `#ifndef ... #endif`.)
* antes de ejecutar el comando sucesivas veces, se debe siempre esperar a que termine la ejecución actual.

### Detalles de implementación
Para este ejercicio, los argumentos están separados por `\n`. Una vez que el argumento fue leído, este es guardado en el heap a través de `strdup`.
Si falla al momento de alocarlo, se notifica el error y se continua leyendo, resultando que se ignore ese argumento para la llamada al comando enviado.

Se agregó también al comienzo del programa una llamada a `isatty` para evitar que el programa se quede leyendo desde la terminal. Solamente acepta si viene desde algún pipeline.
