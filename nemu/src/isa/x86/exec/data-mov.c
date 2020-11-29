#include "cpu/exec.h"

make_EHelper(mov) {
  /*debug bubble sort mov change edx*/
  /*if(cpu.pc==0x1000c5){
    printf("0x%x\n",reg_l(R_EAX));
    printf("%x\n",decinfo.opcode);
    printf("%x\n",id_src->val);
    printf("%d\n",id_dest->reg);
    printf("%d\n",id_dest->type);
  }*/
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //extend imm's sign
  rtl_sext(&s0,&id_dest->val,id_dest->width);
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  operand_write(id_dest,&id_dest->val);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  s0=cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&s0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&s0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  //cpu.esp=s0;

  print_asm("popa");
}

make_EHelper(leave) {
  reg_l(R_ESP)=reg_l(R_EBP);
  rtl_pop(&reg_l(R_EBP));

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    if((int16_t)reg_w(R_AX)>=0)
      reg_w(R_DX)=0x0;
    else
      reg_w(R_DX)=0xffff;
  }
  else {
    if((int32_t)reg_l(R_EAX)>=0)
      reg_l(R_EDX)=0x0;
    else
      reg_l(R_EDX)=0xffffffff;
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    if((int8_t)reg_b(R_AL)>=0)
        reg_b(R_AH)=0x0;
      else
        reg_b(R_AH)=0xff;
  }
  else {
    if((int16_t)reg_w(R_AX)>=0)
      reg_l(R_EAX)=0x0000ffff & reg_l(R_EAX);
    else
      reg_l(R_EAX)=0xffff0000 | reg_l(R_EAX);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

make_EHelper(movsb){
  rtl_lm(&s0,&cpu.esi,1);
  cpu.esi+=1;
  rtl_sm(&cpu.edi,&s0,1);
  cpu.edi+=1;

  print_asm_template2(movsb);
}

make_EHelper(movsw){
  rtl_lm(&s0,&cpu.esi,2);
  cpu.esi+=2;
  rtl_sm(&cpu.edi,&s0,2);
  cpu.edi+=2;

  print_asm_template2(movsw);
}