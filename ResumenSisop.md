# Resumen de Sistemas Operativos

## ¿Qué es un sistema operativo?
* Capa de software que maneja los recursos de la computadora para los usuarios y aplicaciones. Toma un recurso físico y lo transforma en algo mas general y fácil de usar.

### Roles
* Referee: gestionar recursos compartidos
* Ilusionista: abstracción del hardware
* Pegamento: servicios comunes

### Modos de ejecución
* Modo Supervisor: el kernel se ejecuta en este modo
* Modo Usuario: **aislado, protegido, y restringido**

*No confundir con modo root*

## El Kernel

### Ejecución directa
* El programa se ejecuta directamente en la CPU
* La ventaja es la rapidez
* Problemas
  - ¿Como se asegura el OS que el programa no va a hacer nada que el usuario no quiere que sea hecho?
  - ¿Como hace el OS para pausar la ejecución de ese programa y hacer que se ejecute otro?

#### Limites
* Dual Mode Operation: 4 modos de operaciones vía hardware - **Rings** - Del 0 al 3
  - El hardware revisa en que modo se esta ejecutando
  - Bits en el registro de control - Bits IOPL
  - Protege memoria, I/O, ejecución de algunas instrucciones
* Privileged Instructions: Permitido por el modo dual. *Ej de instrucción LGDT*
* Memory Protección: Configurar el hardware para que los procesos solo lean/escriban su memoria.
* Timer Interrupts: Interrumpe el procesador después de un cierto tiempo. **Hardware Counter**
  - Los programas no se quedan con todo el procesamiento 

### Tareas del Kernel
* Planificar ejecución
* Gestionar memoria
* Proveer un Sistema de archivos
* Crear y destruir procesos
* Acceder a dispositivos
* Manejar comunicaciones
* Proveer API para usar el hardware

### Transferencias entre los modos
De Usuario a Kernel
* Interrupciones: Señal enviada al CPU, ocurrió un **evento externo**
* Excepciones del procesador: Causadas por un programa de usuario. *Ej. acceder fuera de la memoria permitida.*
* System Calls: *Funciones* que permiten que el usuario se comunique con el Kernel, para que realice ciertas acciones.

De Kernel a Usuario
* Creación de un nuevo proceso
* Después de manejar una interrupción/excepción/syscall
* Cambio entre procesos (pasando por una interrupción en el medio)

### Tipos de Kernel
* Monolítico: *Ej Linux*
* Micro Kernel: *Ej Minix*

### Inicio
1. Booteo (bootstrap), lo maneja el hardware, realiza chequeos y carga el **bootloader**
2. Se carga el BIOS
3. Se crea el Interrupt Vector Table - En **modo real** (direcciona como máximo 1MB)
4. El BIOS ejecuta la interrupción 19, para ejecutar el servicio de interrupciones
5. Se pasa a **modo supervisor**
6. Se busca al kernel en el dispositivo de almacenamiento y se carga a memoria
7. Se establece el registro de la próxima instrucción
8. Se ejecuta la primera línea del kernel
9. Una vez ejecutado el kernel se cargan a memoria las aplicaciones que deban ejecutarse 

## El Proceso
*"Un proceso es la ejecución de un programa de aplicación con derechos restringidos; el proceso
es la abstracción que provee el Kernel del sistema operativo para la ejecución protegida"- [DAH]*

* *derechos restringidos*: esta en el **ring 3**
* *abstracción*: la virtualizacion que provee el kernel

Un proceso incluye:
* Archivos abiertos
* Señales pendientes
* Datos internos del Kernel
* El estado del procesador
* Un espacio de memoria
* Hilos de ejecución
* Datos globales

### En memoria
* La virtualizacion le hace creer al proceso que tiene toda la memoria disponible para el. *El .code, .data, el heap, y el stack*.
* Cada proceso ve y puede escribir lo suyo, se consigue con la **memoria virtual**
* El proceso cree que tiene todo el CPU también. 
  * Provee un mismo nivel de interfaces mas potentes
  * Aislamiento ante fallas

### API
* fork
* exit, kill
* wait
* exec, sbrk
* getpid, getppid

### El Contexto
* El User Address Space: text, data, stack, heap
* Control Information: Las estructuras *uarea* y *proc*
* Credentials: Ids del proceso
* Variables de entorno
* Hardware Context: Los registros

### Estados posibles
De forma simplificada, esta en 3 estados:
* Running
* Ready
* Blocked

## La Memoria
### Address Space
* El Address Space de un proceso contiene todo el estado de la memoria de un programa en ejecución.
* Cada proceso cree que empieza desde la dirección 0 (virtual), con el hardware (la MMU) se consiguen las direcciones físicas
* Invisible al programa, el OS y el hardware llevan a cabo la ilusión
* No debe de tomar mucho tiempo y debe de usar poco espacio, la memoria virtual debe de ser eficiente
* Debe de otorgar protección entre los procesos

### API
* Stack: reservado y liberado por el compilador
* Heap: La maneja el programador. *malloc()* y *free()* (*brk()* y *sbrk()*)

### Traducción
Hardware-Based Address Translation (Address Translation)

* Transforma una dirección virtual en una física.
* El hardware provee un mecanismo eficiente de bajo nivel
* el sistema operativo setea al hardware de la forma correcta, gerencia la memoria manteniendo información de los lugares libres y en uso, y mantiene el control sobre la memoria usada
* Provee varios beneficios ya mencionados anteriormente

### Implementaciones
#### Base and Bound
* Se tienen 2 registros para la traducción
* PhysicalAddress = VirtualAddress + base
* Tiene que estar dentro del limite puesto por *Bound*
  - Puede tener el tamaño o la dirección tope  

#### Tabla de segmentos
* Se tiene una tabla con varios de estos registros de *Base and Bound* además de permisos de acceso.
* El problema es la fragmentación externa

#### Paginada
* La memoria se reserva en tamaños fijos, *page frames*.
* Se tiene un Índice para acceder a la tabla, el offset se agrega una vez obtenida la dirección física

#### Multinivel
Es un mecanismo mas eficiente de paginación, basado en un árbol o hash table. Provee
* Protección de memoria de grano fino
* Memoria Compartida
* Ubicación de memoria flexible
* Reserva eficiente de memoria
* Un sistema de búsqueda de espacio de direcciones eficiente

#### Segmentación paginada
La memoria esta segmentada, pero en vez de que cada entrada en la pagina de segmentos apunte directamente a una región
continua de la memoria física, cada entrada en la tabla de segmentos apunta a una tabla de paginas,
que a su vez apunta a la memoria correspondiente a ese segmento. La tabla de segmentos
tiene una entrada llamada limite o tamaño que describe la longitud de la pagina de tablas, ósea
la longitud de los segmentos en las paginas

### TLB
* Mejora de rendimiento, es un cache de traducciones
* Por cada acceso a memoria, se revisa primero la TLB
  - TLB Hit si esta
  - TLB Miss caso contrario, se busca a memoria la tabla para realizar la traducción y se trae a la TLB (principios de localidad espacial y temporal)
* Tener en cuenta la consistencia con los datos originales cuando los datos son modificados
* En un Context Switch las entradas de la TLB ya no son validas. Se le realiza un flush a la TLB
* La consistencia de la TLB la mantiene el OS
* En multiprocesador, cada vez que una entrada se modifica de un CPU, se descarta el del resto. Se llama TLB Shootdown

## Scheduling o Planificación de Procesos
* Hay que tener una forma de poder elegir que proceso se ejecuta cuando se tienen varios. Se encarga un planificador.
* Debe de ser lo más eficiente posible, *tender a O(1)*.
* La multiprogramación permite que si algún proceso esta haciendo I/O, otro proceso haga cómputo.
* Multiple Fixed Partitions: Se realizaban particiones de la memoria en procesos grandes y chicos.
* Multiple Variable Partitions: Cada tarea entra a memoria y ocupa un espacio. Se puede realocar. Elimina fragmentación interna y externa.
* Workload: Es la carga de trabajo de un proceso corriendo en el sistema.

### Métricas
#### Turnaround

T_*turnaround* = T_*completion* - T_*arrival*

#### Response Time

T_*response* = T_*firstrun* - T_*arrival*

### Políticas
#### FIFO
* Se ejecuta el primero en llegar
* La puede afectar el Convoy Effect
* Non-preemptive (desalojo por parte del kernel)

#### Shortest Job First - SJF
* Ejecuta el trabajo más corto primero
* Sigue afectando el Convoy Effect
* Non-preemptive

#### Shortest Time-to-Competion - STC
* Si hay algún proceso que puede terminar antes ejecuta ese
* Pre-emptive

#### Round-Robin - RR
* Se ejecutan los procesos en un *time slice*
* Todos por igual
* Necesita un buen *time slice*
* Mejora *response time*, empeora *turnaround time*

#### Multi-Level Feedback Queue - MLFQ
* Intenta optimizar el *turnaround time* ejecutando la tarea más corta primero
* Intenta reducir el *response time*, haciendo que tenga un tiempo interactivo para los usuarios
* Establece un conjunto de colas, cada una con una prioridad
* Queremos
  - Si una tarea no usa la CPU, tiene prioridad alta (proceso interactivo)
  - Si usa mucho la CPU, prioridad baja
* Problemas que puede tener
  - Starvation, demasiadas tareas interactivas, no se ejecutan las de cómputo
  - Explotar el sistema por parte del programador, escribiendo a un archivo antes de que termine el *time slice*
  - Programas dinámicos, pueden pasar de mucho cómputo a interactivos
* Reglas, A y B son 2 procesos
  1. Si la prioridad de A es mayor a la de B, se ejecuta A, B no se ejecuta
  2. Si A y B tienen igual prioridad, se ejecutan en Round Robin
  3. Si entra un proceso nuevo, se le asigna la prioridad más alta
  4. Una vez usada su asignación de tiempo en un nivel dado, su prioridad se reduce independientemente de si renuncio al CPU. Esto evita que los programadores tomen ventaja del planificador.
  5. Después de un cierto tiempo se hace un **boost de prioridad**. Arregla el Starvation y los programas dinámicos.
* La constante S (para hacer el boost) es una VOO-DOO CONSTANT, difícil de determinar

#### Proportional Sharing
* Intenta que cada tarea obtenga un porcentaje de uso de tiempo del CPU.
* Se conoce también como planificación por lotería. Se realiza un sorteo para identificar quien se ejecuta a continuación.
* La visión es correcta desde el punto de vista probabilístico, pero no garantiza el resultado.
* Se pueden manipular los boletos con algunos mecanismos:
  - Ticket Currency: Serian distintos tipos de moneda
  - Transferencia de boletos: Un proceso le transfiere a otro
  - Inflación: Aumenta o disminuye la cantidad de boletos que tenga un proceso de forma temporal
* La implementación es muy sencilla, requiere de:
  - Un generador de números aleatorios que determine el ganador
  - Una estructura para guardar la información de los procesos
  - Un total de tickets

### En multiprocesador
* Los planificadores no mejoran el rendimiento delas aplicaciones haciéndolas multithreading, esto depende del programador.
* Hay que tener cuidado con el caché, conviene mantener los procesos en el mismo CPU, ya que se arma un estado que conviene guardar. Se conoce como Afinidad de Caché.
* Hay que tener cuidado también entre varios CPUs, de que se mantenga la *coherencia de caché*. Se utiliza *Bus Snooping*

#### Single Queue Multiprocessor Scheduling - SQMS
* Forma más fácil, es simple y no requiere trabajo de adaptar la política existente a más de una CPU.
* No es escalable, se pierde la afinidad del caché
* Hay que bloquear el acceso a la cola (locks), lo cual reduce el rendimiento.

#### Multi-Queue Multiprocessor Scheduling - MQMS
* Múltiples colas, cada una sigue una disciplina de planificación.
* Es escalable.
* Saca el problema de los bloqueos y del caché.
* Provee afinidad del caché intrínsecamente, las tareas intentan mantenerse en el mismo CPU que fueron ejecutadas
* Tiene el problema del **desbalance de ejecución** (load imbalance), sucede cuando una CPU está sobrecargada de procesos mientras que otra no.
  - Se puede *arreglar* con la técnica **migration**

### Completly Fair Scheduler - CFS
* Es el planificador de Linux
* No otorga un determinado *time slice* a un proceso, sino que otorga una proporción del procesador dependiendo de la carga del sistema.
* Intenta gastar poco tiempo en las decisiones de planificación, lo consigue por el diseño y las estructuras de datos usadas.
* Divide el tiempo entre los procesos compitiendo por él. **Vruntime**, es el tiempo que corrió cada proceso normalizado por el número de procesos *runnable*.
* No deben de correr muy poco, para no perder performance con los *context switches*.
* No debe de cambiar poco, para mantener la equidad entre procesos, **fairness*
* Maneja esto con algunos parámetros:
  - **sched_latency**: Determina cuanto tiempo un proceso tiene que ejecutarse antes de considerar un switch. (time slice dinámico) Es dividido entre todos los procesos ejecutándose en la CPU
  - **min_granularity**: Establece un valor mínimo del time-slice, asegurándose que nadie tenga menos de este tiempo de ejecución y de que no haya un overhead por el context switch
* Tiene interrupciones periódicas para tomar decisiones.
* Se pueden asignar prioridades, del -20 a +19. 0 es por defecto. -20 es más prioridad, +19 menos.
* Utiliza árboles rojo-negro
  1. El nodo más a la izquierda es elegido, es el que menos tiempo se ejecutó
  2. Si el proceso termina, se elimina del árbol
  3. Si alcanza su máximo tiempo de ejecución, o para su ejecución voluntariamente, se reinserta en el árbol en base al nuevo tiempo
  4. Repetir para el nodo más a la izquierda

* Su API vendria a ser *nice()* y *sched_yield()*

## Concurrencia
* Un *thread* es una secuencia de ejecución atomiza que representa una tarea planificable de ejecución. (dentro de un programa - diferencia con un proceso)
  - Secuencia de ejecución atómica porque cada thread ejecuta una secuencia de instrucciones, un bloque de código
  - Tarea planificable, ya que el OS decide cuando ejecutarlo, suspenderlo, o continuarlo
* No confundir concurrencia con paralelismo.
  - Concurrencia es pretender estar haciendo varias cosas a la vez, pero siempre se le dedica tiempo a una
  - Paralelismo es realizar varias a la vez 
* Cuantan con varias caracteristicas
  - Thread ID
  - Conjunto de valores de registros
  - Stack propio
  - Politica y prioridad de ejecucion
  - *errno* propio
  - Datos especificos del thread
* Es necesaria alguna forma de planificar los *threads*, pueden ejecutarse de forma cooperativa (no hay interrupción a menos que se solicite), o de forma preemptiva (puede salir de running en cualquier momento)
* API de *pthread*
  
  ```C
  int pthread_create(pthread_t * thread, const pthread_att_t * att,void * (start_routine) (void *), void * arg)
  ```
     1. thread: Es un puntero a la estructura de tipo pthread_t, que se utiliza para interactuar con el threads.
     2. attr: Se utiliza para especificar los ciertos atributos que el thread deberia tener, por ejemplo, el tamaño del stack, o la prioridad de scheduling del thread. En la mayoria de los casos es NULL.
     3. start_routine: Sea tal vez el argumento más complejo, pero no es más que un puntero a una función, en este caso que devuelve void.
     4. arg: Es un puntero a void que debe apuntar a los argumentos de la función.
  
  ```C
  int pthread_join(pthread_t thread, void **value_ptr )
  ```
  1. thread es el thread por el que hay que esperar y es de tipo pthread_t.
  2. value_ptr es el puntero al valor esperado de retorno.

Tener en cuenta que:
  - Si la función no devuelve nada NULL.
  - Si solo devuelve un valor no hay que hacer el empaquetado de los punteros.
  - Nunca devolver nada que se encuentre allocated dentro del thread.

Otras funciones de la API:
```C
pthread_exit(status);
phtread_cancel(thread);
pthread_detach(threadid);
```

* Los *threads* se usan porque pueden acelerar el rendimiento del programa
* Si se realiza I/O, otro puede realizar otras operaciones
* Hay que tener cuidado con los recursos compartidos

### Estado Per-Thread y Threads Control Block
Cada thread debe tener una estructura que represente su estado. Esta estructura se denomina
Thread Control Block (TCB), se crea una entrada por cada thread. La TCB almacena el estado
per-thread de un thread. Este bloque debe almacenar el puntero al stack del thread y una copia
de los registros en el procesador. De esta forma el sistema operativo puede crear, parar, y volver
a iniciar múltiples threads sin problemas de estado de cada uno.
  
De forma compartida se debe guardar también información, el código, variables globales, variables del heap.

### Estados de un thread
* INIT
* READY
* RUNNING
* WAITING
* FINISHED

### Diferencias con procesos
Los threads
* Por defecto comparten memoria
* Por defecto comparten los descriptores de archivos
* Por defecto comparten el contexto del filsystem
* Por defecto comparten el manejo de señales
Los Procesos
* Por defecto no comparten memoria
* Por defecto no comparten los descriptores de archivos
* Por defecto no comparten el contexto del filsystem
* Por defecto no comparten el manejo de señales

Linux no diferencia entre procesos y threads, son una tarea ejecutándose.
* Son creados a través de la *syscall* *clone()*, se le pasan diferentes flags dependiendo de lo que se quiera compartir

### Sincronizacion
* Se puede ejecutar de forma independiente (no comparten recursos), o cooperativa (comparten recursos)
* Hay que tener cuidado en el caso cooperativo, la ejecución del programa depende de como se intercale la ejecución. No es determinístico
* Las **Race Conditions** se dan cuando el resultado del programa depende de como se intercalaron las operaciones de los *threads*
* Las **Operaciones Atómicas** son aquellas que no pueden dividirse en otras al momento del ensamblado. Garantizan la ejecución sin tener que intercalarse
* Los programas tienen que ser
  - **Safety**: seguros, nada malo va a pasar
  - **Liveness**: si algo va a pasar, tiene que ser bueno
* Los **Locks** son variables que permiten la sincronización mediante la exclusión mutua, si un thread lo tiene, ningún otro lo puede tener.
  - Están asociados a estados o partes de código
  - Estados: *busy* y *free*, su cambio y verificación son atómicos
* La Sección Crítica es aquella sección del código fuente que se necesita que se ejecute en forma atómica. Para ello esta sección se encierra dentro de un lock.
* El sistema operativo no puede inferir donde poner un lock en un programa

La API de *pthreads*
```C
pthread_mutex_t lock;
rc=pthread_mutex_init(&lock,NULL);
assert(rc==0);

pthread_mutex_lock(&lock);
x=x+1;
pthread_mutex_unlock(&lock);
```

* Las **Condition Variables** permiten que un *thread* espere a otro *thread* para tomar una acción, es más eficiente que mandarlos a girar (*spinning*) en un ciclo. Se usan para dar algún tipo de señal entre *treads*

```C
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```

```C
pthread_mutex_lock(&lock);
while (ready == 0)
  pthread_cond_wait(&cond, &lock);
pthread_mutex_unlock(&lock);
```

```C
pthread_mutex_lock(&lock);
ready = 1;
pthread_mutex_unlock(&lock);
pthread_cond_signal(&cond);
```

### Errores comunes en concurrencia
* Non Deadlocks
  - Atomicity violation: “El deseo de la serialización entre múltiples accesos a memoria es violado”
  - Order Violation: “El orden deseado entre accesos a memoria se ha cambiado”

* Deadlocks
  - Ocurre cuando dos o más threads se bloquean entre sí.
  - Se puede dar cuando uno obtiene el lock, y por algun motivo nunca lo libera (exclusión mutua).
  - Hold and wait, un thread mantiene un recurso reservado para sí mismo mientras espera que se de alguna condición. Se previene haciendo que los locks se tomen en forma atómica.
  - No preemption, los recursos adquiridos no pueden ser desalojados por la fuerza (preempted).
  - Circular wait, un conjunto de threads que de forma circular cada uno reserva recursos que necesita otro thread. Se previene escribiendo un programa que no tenga esperas circulares (Ej. Agregando un orden*)

## Sistema de archivos
* Permite a un usuario organizar sus datos para que puedan persistir en el tiempo
* *Una abstracción del sistema operativo que provee datos persistentes con un nombre*
* El nombre esta para facilitar la identificación, pero no es necesario para tener un sistema de archivos.

### Virtual File System - VFS
* Subsistema del kernel que implementa las interfaces que tienen que ver con los archivos y el sistema de archivos provisto a los programas corriendo en modo usuario.
* Los sistemas de archivos deben basarse en el VFS.
* *VFS es el pegamento que habilita a las system calls como por ejemplo open(), read() y write() a funcionar sin que estas necesiten tener en cuenta el hardware subyacente*.
* Provee un modelo común de archivos que pueda representar cualquier característica y comportamiento general de cualquier sistema de archivos.

#### File System Abstraction Layer
* Interfaz generica
* Habilita a Linux soportar sistemas de archivos diferentes (incluso en caracteristicas y comportamiento)
* Posible por el VFS
* Define interfaces conceptualmente basicas y estructuras que cualquier sistema de archivos soporta
* Cada *filesystem* se adapta al VFS
* El resultado es que el kernel puede manejar muchos tipos de sistemas de archivos de forma fácil y limpia.

#### Estructuras
* Super Bloque: representa a todo un sistema de archivos, (*Ej. size, block size, la ubicación de las tablas de inodos*)
* Inodo: representa un determinado archivo dentro de un sistema de archivos, metadata acerca de los archivos. *Ej. Tamaño, fecha de modificación, propietario, seguridad*
* Dentry: representa una entrada de directorio, es un componente simple de un path, relaciona los inodos con los nombres
* File: representa un archivo asociado a un determinado proceso. Los datos de un archivo están en la data región como bytes sin tipo.
Los directiorios se tratan como archivos.

#### Operaciones
* super_operations: Son métodos que aplica el kernel sobre un determinado sistema de archivos. *Ej. write_inode() o sync_fs().*
* inode_operations:  Son métodos que aplica el kernel sobre un archivo determinado. *Ej. create() o link().*
* dentry_operations: Son métodos que se aplican directamente por el kernel a una determinada directory entry. *Ej. d_compare() o d_delete(), compara elimina directorios.*
* file_operations: Son métodos que aplica el kernel sobre un archivo abierto por un proceso. *Ej. read() o write()*

#### API
Para archivos:
* open()
* creat()
* close()
* read()
* write()
* lseek()
* dup()
* link()
* unlink()

Para directorios:
* mkdir()
* rmdir()
* chdir()
* opendir()
* readdir()
* closedir()

Sobre los metadatos:
* stat()
* access()
* chmod()
* chown()
* umask()

Se pueden ver ejemplos de usos de *syscalls* en los ejercicios realizados o en el lab fork.
