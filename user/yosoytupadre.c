
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("PID actual = %d\n", getpid());
  printf("PID del padre = %d\n", getppid());

  int k = 0;
  while (1) {
    int anc = getancestor(k);
    printf("ancestor(%d) = %d\n", k, anc);
    if (anc == -1) break;
    k++;
  }
  exit(0);
}
