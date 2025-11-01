#include "kernel/syscall.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


static void wait_ticks(int dt){
  int start = uptime();
  while (uptime() - start < dt){
    
    yield();
  }
}

static void spin_loop(void){
  volatile uint x = 0;
  for(;;){
    x += 1;
    if((x & 0x3FFFFF) == 0) yield();
  }
}

int main(void){
  int N = 10;
  int tickets[N];
  int pids[N];

  for(int i = 0; i < N; i++) tickets[i] = 50 * (i + 1); 

  printf("demo: creando %d procesos...\n", N);

  for(int i = 0; i < N; i++){
    int pid = fork();
    if(pid < 0){ printf("fork fallo\n"); exit(1); }

    if(pid == 0){
      // hijo
      settickets(tickets[i]);
      spin_loop();  
      exit(0);
    } else {
      pids[i] = pid;
    }
  }

  
  wait_ticks(400);

  printf("\n= Slices en ventana fija =\n");
  for(int i = 0; i < N; i++){
    int s = getslices(pids[i]);  
    printf("pid %d  tickets=%d  slices=%d\n", pids[i], tickets[i], s);
  }

  
  for(int i = 0; i < N; i++) kill(pids[i]);
  for(int i = 0; i < N; i++) wait(0);

  printf("\nFin demo.\n");
  exit(0);
}

