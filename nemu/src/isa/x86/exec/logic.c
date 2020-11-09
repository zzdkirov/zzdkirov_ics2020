#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&t0,&id_dest->val,&id_src->val);
  reg_f(CF)=0;
  reg_f(OF)=0;
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  reg_f(CF)=0;
  reg_f(OF)=0;
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);

  reg_f(CF)=0;
  reg_f(OF)=0;

  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  reg_f(CF)=0;
  reg_f(OF)=0;
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
