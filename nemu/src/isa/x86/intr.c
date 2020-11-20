#include "rtl/rtl.h"
#include "cpu/exec.h"
#include "isa/mmu.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //push eflags IF=0; cs retaddr jmp 2 target

  rtl_push(&cpu.eflags.value);
  cpu.eflags.IF=0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);

  vaddr_t irpt_table_addr=cpu.idtr.base + sizeof(GateDesc)*NO;
  vaddr_t low_addr=vaddr_read(irpt_table_addr,2)&0xffff;
  vaddr_t high_addr=vaddr_read(irpt_table_addr+4,2)&0xffff0000;

  decinfo.jmp_pc=high_addr|low_addr;
  decinfo.is_jmp=1;
  rtl_j(decinfo.jmp_pc);
}

bool isa_query_intr(void) {
  return false;
}
