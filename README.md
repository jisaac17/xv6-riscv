Se modificó el xv6 para implementar el lottery schedulling.
Lo que conlleva que cada proceso posee una cantidad de tickets para representar la probabilidad de se>

Lógica a seguir
- en runnable se suman los tickets de todos los procesos
- se genera un número aleatorio r entre 1 y el total
- se revisan la lista de procesos hasta que uno se acum ≥ r
- el proceso encontrado es el ganador de la “lotería” y se ejecuta
- Antes de la conmutación de contexto, se incrementa su contador slices

Siguiendo esta lógica ocurre que más tickets mayor probabilidad de uso del CPU

Siguiendo esta logica ocurre que mas tickest mayor probabilidad de uso del CPU

##Archivos modificados

- kernel/proc.h: se agrego int tickets u uint slices
- kernel/proc.c: allocprocm inicializa las variables
- kernel/sysproc.c: se implementan los procesos de tickets y de slices
- kernel/syscall.h: nuevos números ligados a las nuevas sycalls
- user/usys.pl: entrys correspondientes para dar disponibilidad de user-space
- kernel/syscall.c: nuevas entradas en la tabla
- user/user.h: declaraciones de las sycalls
- user/demo.c: programa para probar el scheduler

##Dificultades encontradas

argint() no devolvía el valor, ajustamos la implementación para que argint sea void en esta versión.
También se intentó usar sleep() y no existía en esta variante de XV6 se creó sycall yield() para ceder CPU

##Potenciales problemas de lotería scheduling

- alta variabilidad de aleatoriedad, afecta a la distribución de la prioridad y finalmente a la experiencia del usuario
- son necesarias ventanas medianas o largas de tiempo para que la proporcionalidad se logre.
- Malwares u otros procesos maliciosos pueden exigir demasiados tickets.
