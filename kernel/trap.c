#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct spinlock tickslock;
uint ticks;

extern char trampoline[], uservec[];

void kernelvec();

extern int devintr();

void
trapinit(void)
{
  initlock(&tickslock, "time");
}

void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}

static uint64
get_reg(struct trapframe *tf, int reg)
{
  if(reg == 0) return 0;
  
  switch(reg) {
    case 1:  return tf->ra;
    case 2:  return tf->sp;
    case 3:  return tf->gp;
    case 4:  return tf->tp;
    case 5:  return tf->t0;
    case 6:  return tf->t1;
    case 7:  return tf->t2;
    case 8:  return tf->s0;
    case 9:  return tf->s1;
    case 10: return tf->a0;
    case 11: return tf->a1;
    case 12: return tf->a2;
    case 13: return tf->a3;
    case 14: return tf->a4;
    case 15: return tf->a5;
    case 16: return tf->a6;
    case 17: return tf->a7;
    case 18: return tf->s2;
    case 19: return tf->s3;
    case 20: return tf->s4;
    case 21: return tf->s5;
    case 22: return tf->s6;
    case 23: return tf->s7;
    case 24: return tf->s8;
    case 25: return tf->s9;
    case 26: return tf->s10;
    case 27: return tf->s11;
    case 28: return tf->t3;
    case 29: return tf->t4;
    case 30: return tf->t5;
    case 31: return tf->t6;
    default: return 0;
  }
}

uint64
usertrap(void)
{
  int which_dev = 0;
  int handled_writeonly = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  
  p->trapframe->epc = r_sepc();
  
  uint64 scause = r_scause();
  
  // Manejo de page faults para paginas write-only
  if(scause == 13 || scause == 15) {
    uint64 va = r_stval();
    uint64 va_page = PGROUNDDOWN(va);
    pte_t *pte = walk(p->pagetable, va_page, 0);
    
    if(pte && (*pte & PTE_V) && (*pte & PTE_WO)) {
      handled_writeonly = 1;
      
      if(scause == 15) {
        // Escritura permitida - emular
        uint64 pa = PTE2PA(*pte) + (va - va_page);
        
        uint64 epc = p->trapframe->epc;
        uint64 epc_page = PGROUNDDOWN(epc);
        pte_t *epc_pte = walk(p->pagetable, epc_page, 0);
        
        if(epc_pte == 0 || (*epc_pte & PTE_V) == 0) {
          setkilled(p);
        } else {
          uint64 epc_pa = PTE2PA(*epc_pte) + (epc - epc_page);
          uint32 instr = *(uint32*)epc_pa;
          
          int rs2 = (instr >> 20) & 0x1F;
          int funct3 = (instr >> 12) & 0x7;
          uint64 val = get_reg(p->trapframe, rs2);
          
          if(funct3 == 0)
            *(uint8*)pa = (uint8)val;
          else if(funct3 == 1)
            *(uint16*)pa = (uint16)val;
          else if(funct3 == 2)
            *(uint32*)pa = (uint32)val;
          else if(funct3 == 3)
            *(uint64*)pa = val;
          
          p->trapframe->epc += 4;
        }
        
      } else {
        // Lectura bloqueada
        setkilled(p);
      }
    }
  }
  
  if(scause == 8){
    if(killed(p))
      kexit(-1);

    p->trapframe->epc += 4;
    intr_on();
    syscall();
    
  } else if((which_dev = devintr()) != 0){
    // ok
    
  } else if(!handled_writeonly && (scause == 15 || scause == 13)) {
    if(vmfault(p->pagetable, r_stval(), (scause == 13) ? 1 : 0) != 0) {
      printf("usertrap(): unexpected scause 0x%lx pid=%d\n", scause, p->pid);
      printf("            sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
      setkilled(p);
    }
    
  } else if(!handled_writeonly && scause != 8) {
    printf("usertrap(): unexpected scause 0x%lx pid=%d\n", scause, p->pid);
    printf("            sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if(killed(p))
    kexit(-1);

  if(which_dev == 2)
    yield();

  prepare_return();

  uint64 satp = MAKE_SATP(p->pagetable);

  return satp;
}

void
prepare_return(void)
{
  struct proc *p = myproc();

  intr_off();

  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  p->trapframe->kernel_satp = r_satp();
  p->trapframe->kernel_sp = p->kstack + PGSIZE;
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();

  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP;
  x |= SSTATUS_SPIE;
  w_sstatus(x);

  w_sepc(p->trapframe->epc);
}

void 
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  
  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){
    printf("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(), r_stval());
    panic("kerneltrap");
  }

  if(which_dev == 2 && myproc() != 0)
    yield();

  w_sepc(sepc);
  w_sstatus(sstatus);
}

void
clockintr()
{
  if(cpuid() == 0){
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
  }

  w_stimecmp(r_time() + 1000000);
}

int
devintr()
{
  uint64 scause = r_scause();

  if(scause == 0x8000000000000009L){
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000005L){
    clockintr();
    return 2;
  } else {
    return 0;
  }
}
