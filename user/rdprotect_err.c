#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

int
main(void)
{
  // Reservamos una página bien mapeada y alineada
  char *base = sbrk(0);
  sbrk(PGSIZE);

  // En xv6 normalmente sbrk mantiene alineado, pero por si acaso:
  uint64 aligned = ((uint64)base + PGSIZE - 1) / PGSIZE * PGSIZE;
  base = (char *)aligned;

  int ret;

  // 1) len = 0
  ret = mrdprotect(base, 0);
  printf("ERR: len = 0 → %s (ret = %d)\n", ret < 0 ? "OK (falló como debía)" : "MAL (no falló)", ret);

  // 2) len < 0
  ret = mrdprotect(base, -1);
  printf("ERR: len < 0 → %s (ret = %d)\n", ret < 0 ? "OK (falló como debía)" : "MAL (no falló)", ret);

  // 3) addr no alineada a página
  char *unaligned = base + 1;
  ret = mrdprotect(unaligned, 1);
  printf("ERR: addr no alineada → %s (ret = %d)\n", ret < 0 ? "OK (falló como debía)" : "MAL (no falló)", ret);

  // 4) dirección fuera del espacio de usuario / no mapeada
  //    Tomamos el fin actual del heap y redondeamos hacia arriba a la siguiente página,
  //    que no ha sido reservada con sbrk, por lo tanto no está mapeada.
  char *heap_end = sbrk(0);
  uint64 unmapped_aligned = ((uint64)heap_end + PGSIZE - 1) / PGSIZE * PGSIZE;
  char *unmapped = (char *)unmapped_aligned;

  ret = mrdprotect(unmapped, 1);
  printf("ERR: dirección fuera de espacio usuario / no mapeada → %s (ret = %d)\n",
         ret < 0 ? "OK (falló como debía)" : "MAL (no falló)", ret);

  // 5) dirección claramente "kernel" (muy alta)
  char *kernel_addr = (char *)(0xFFFFFFFFFFFFF000ULL);
  ret = mrdprotect(kernel_addr, 1);
  printf("ERR: dirección tipo kernel → %s (ret = %d)\n",
         ret < 0 ? "OK (falló como debía)" : "MAL (no falló)", ret);

  exit(0);
}
