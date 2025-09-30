# Tarea 1: Implementación de Llamadas al Sistema en xv6

Se crearon dos sycalls nuevas en xv6:

- getpid() que retorna el PID del proceso padre.

- getancestor(): que retorna el PID del ancestro "n" hasta devuelve -1 es decir que el numero indicado no es valido  y por lo tanto no hay mas ancestros

Ademas de estas dos llamadas se implemento un programam de prueba con  el nombre de "yosoytupadre"

##Funcionamiento de las llamadas al sistema

Las nuevas syscalls utilizan la estructura `proc`:

-getpid() obtiene el proceso actual con `myproc()`, revisa el campo parent y retorna su pid.
-getancestor(n) parte del proceso latente y revisa el mismo campo parent "n" veces hasta que se rompe el ciclo cuando el padre no es valido,
lo que se verifica cuando se retorna -1 como se menciono anteriormente.
Ademas se manejaron los casos borde como procesos sin padre (ej. `init`) y argumentos negativos.

##Explicación de las modificaciones realizadas.
 
- `kernel/syscall.h`: definición de números de syscall correspondientes (SYS_getppid y SYS_getancestor).
- `user/user.h`: se incluyeron las funciones de usuario (int getppid(void); y int getancestor(int n);).
- `user/usys.pl`: entradas para generar stubs en ensamblador utilizando entry.
- `kernel/syscall.c`: agregado de prototipos y entradas en la tabla syscalls[].
- `kernel/sysproc.c`: implementación de sys_getppid y sys_getancestor.
- `user/yosoytupadre.c`: programa de prueba que llama a ambas sycalls.
- `Makefile`: agregado de _yosoytupadre en la lista UPROGS.

## Dificultades enfrentadas
- **Uso de `argint`**: en xv6-riscv esta función retorna void, lo que requirió ajustar el código para no comparar con < 0.
- **Errores de Makefile**: al momento de modificar la lista UPROGS se utilizaron barras invertidas `\` que no permitieron compilar hasta arreglar el error



