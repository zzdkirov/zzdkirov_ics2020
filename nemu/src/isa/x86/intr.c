#include "rtl/rtl.h"
#include "cpu/exec.h"
#include "isa/mmu.h"
//extern int printf(const char* fmt,...);

#define IRQ_TIMER 32

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //push eflags ; cs retaddr jmp 2 target

  rtl_push(&cpu.eflags.value);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);

  vaddr_t low_addr=vaddr_read(cpu.idtr.base + sizeof(GateDesc)*NO,4)&0x0000ffff;
  vaddr_t high_addr=vaddr_read(cpu.idtr.base + sizeof(GateDesc)*NO+4,4)&0xffff0000;
  rtl_j(high_addr|low_addr);
}

bool isa_query_intr(void) {
  if(cpu.INTR & cpu.eflags.IF){
    cpu.INTR = false;
    //time irpt ret addr== now pc
    raise_intr(IRQ_TIMER,cpu.pc);
    return true;
  }

  return false;
}
