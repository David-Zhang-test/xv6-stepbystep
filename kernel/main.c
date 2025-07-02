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
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");

    
    printf("cpu %d is booting!\n", cpuid());
    __sync_synchronize();
    started = 1;
  } else {
  while(started == 0);
  __sync_synchronize();
  printf("cpu %d is booting!\n", cpuid());
  }
  while (1);
}