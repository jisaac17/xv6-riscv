#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile char *vaddr;

int main() {
  char *addr = sbrk(0);
  sbrk(4096);
  
  vaddr = addr;
  vaddr[0] = 'Z';
  
  if (mrdprotect(addr, 1) < 0) {
    printf("mrdprotect fallo\n");
    exit(1);
  }
  
  vaddr[0] = 'A';
  
  char c = vaddr[0];
  printf("Valor leido: %c (esto NO deberia imprimirse)\n", c);
  
  if (munrdprotect(addr, 1) < 0) {
    printf("munrdprotect fallo\n");
    exit(1);
  }
  
  printf("Proteccion revertida correctamente.\n");
  exit(0);
}
