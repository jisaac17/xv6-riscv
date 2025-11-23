#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  char *addr = sbrk(0);      // Dirección actual del heap
  sbrk(4096);                // Reservar una página

  addr[0] = 'Z';
  printf("SAFE: Antes de proteger: addr[0] = %c\n", addr[0]);

  if (mrdprotect(addr, 1) < 0) {
    printf("SAFE: mrdprotect falló\n");
    exit(1);
  }

  printf("SAFE: Después de mrdprotect (NO tocamos addr mientras está protegida).\n");

  // Aquí NO leemos ni escribimos addr[0] mientras está protegida

  if (munrdprotect(addr, 1) < 0) {
    printf("SAFE: munrdprotect falló\n");
    exit(1);
  }

  printf("SAFE: Después de munrdprotect, vamos a escribir y leer...\n");

  addr[0] = 'A';        // debería funcionar
  char c = addr[0];     // debería funcionar
  printf("SAFE: Lectura OK después de revertir: %c\n", c);

  exit(0);
}
