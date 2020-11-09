#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &(id_src->val));
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
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

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
