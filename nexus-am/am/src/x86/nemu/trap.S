#----|------------entry------------|---irq id---|-----handler-----|
.globl __am_vecsys;    __am_vecsys: pushl $0x80; jmp __am_asm_trap
.globl __am_vectrap;  __am_vectrap: pushl $0x81; jmp __am_asm_trap
.globl __am_irq0;        __am_irq0: pushl   $32; jmp __am_asm_trap
.globl __am_vecnull;  __am_vecnull: pushl   $-1; jmp __am_asm_trap

__am_asm_trap:
  pushal  #protect all regs

  pushl $0   #protect addr space

  pushl %esp  #param trans
  call __am_irq_handle

  mov %eax, %esp  #send eax 2 esp ,to ctrl context

  #addl $4, %esp   #ignore the param, debug final

  addl $4, %esp   #ignore the addr space
  popal   #recover all regs
  addl $4, %esp

  iret
