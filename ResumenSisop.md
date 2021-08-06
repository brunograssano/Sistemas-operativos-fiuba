# Resumen de Sistemas Operativos

## ¿Que es un sistema operativo?
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

## Scheduling o Planificacion de Procesos
* Hay que tener una forma de poder elegir que proceso se ejecuta cuando se tienen varios. Se encarga un planificador.
* Debe de ser lo mas eficiente posible, *tender a O(1)*.
* La multiprogramacion permite que si algun proceso esta haciendo I/O, otro proceso haga computo.
* Multiple Fixed Partitions: Se realizaban particiones de la memoria en procesos grandes y chicos.
* Multiple Variable Partitions: Cada tarea entra a memoria y ocupa un espacio. Se puede realocar. Elimina fragmentacion interna y externa.
* Workload: Es la carga de trabajo de un proceso corriendo en el sistema.

### Metricas
#### Turnaround

T_*turnaround* = T_*completion* - T_*arrival*

#### Response Time

T_*response* = T_*firstrun* - T_*arrival*

### Politicas
#### FIFO
* Se ejecuta el primero en llegar
* La puede afectar el Convoy Effect
* Non-preemptive (desalojo por parte del kernel)

#### Shortest Job First - SJF
* Ejecuta el trabajo mas corto primero
* Sigue afectanto el Convoy Effect
* Non-preemptive

#### Shortest Time-to-Competion - STC
* Si hay algun proceso que puede terminar antes ejecuta ese
* Pre-emptive

#### Round-Robin - RR
* Se ejecutan los procesos en un *time slice*
* Todos por igual
* Necesita un buen *time slice*
* Mejora *response time*, empeora *turnaround time*

#### Multi-Level Feedback Queue - MLFQ
* Intenta optimizar el *turnaround time* ejecutando la tarea mas corta primero
* Intenta reducir el *response time*, haciendo que tenga un tiempo interactivo para los usuarios
* Establece un conjunto de colas, cada una con una prioridad
* Queremos
  - Si una tarea no usa el CPU, tiene prioridad alta (proceso interactivo)
  - Si usa mucho el CPU, prioridad baja 
* Problemas que puede tener
  - Starvation, demasiadas tareas interactivas, no se ejecutan las de computo
  - Explotar el sistema por parte del programador, escribiendo a un archivo antes de que termine el *time slice*
  - Programas dinamicos, pueden pasar de mucho computo a interactivos 
* Reglas, A y B son 2 procesos
  1. Si la prioridad de A es mayor a la de B, se ejecuta A, B no se ejecuta
  2. Si A y B tienen igual prioridad, se ejecutan en Round Robin
  3. Si entra un proceso nuevo, se le asigna la prioridad mas alta
  4. Una vez usada su asignación de tiempo en un nivel dado, su prioridad se reduce independientemente de si renuncio al CPU. Esto evita que los programadores tomen ventaja del planificador.
  5. Despues de un cierto tiempo se hace un **boost de prioridad**. Arregla el Starvation y los programas dinamicos.
* La constante S (para hacer el boost) es una VOO-DOO CONSTANT, dificil de determinar

#### Proportional Sharing

*wip*






