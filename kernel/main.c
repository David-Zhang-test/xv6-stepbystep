#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void main()
{
  if(cpuid() == 0) {
    // 此处为调用 printf()执行必要的初始化
    uartinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");

    
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    virtio_disk_init(); // emulated hard disk
    userinit();     // first user process



    printf("cpu %d is booting!\n", cpuid());
    __sync_synchronize();
    started = 1;
    
  } else {
  while(started == 0);
  __sync_synchronize();
  printf("cpu %d is booting!\n", cpuid());
  kvminithart();    // turn on paging
  trapinithart();   // install kernel trap vector
  plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();

}