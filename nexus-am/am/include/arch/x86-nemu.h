#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context {
  //first push eflags,cs ,ret addr, then push irq num, then pusha,then push space addr (in pa3 is 0)

  struct _AddressSpace *as;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip,cs,eflags;
};

#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
