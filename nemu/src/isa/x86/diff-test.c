#include "nemu.h"
#include "monitor/diff-test.h"

extern void difftest_getregs(void *r);
extern const char *regsl[];

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  uint32_t r[10];
	difftest_getregs(r);
	CPU_state *ref_ptr = ref_r;
	int flag = 0;
	int i;
	for(i=0;i<8;i++){
		if(r[i]!=*((uint32_t *)ref_ptr+i)){
			printf("diff:%s %08x in NEMU %08x in QEMU\n ", regsl[i], r[i], *((uint32_t *)ref_ptr+i));
			flag = 1;
		}
		else{
			//printf("nodiff:%s %08x in NEMU %08x in QEMU\n ", regsl[i], r[i], *((uint32_t *)ref_ptr+i));
		}
	}


	//uint32_t eflags_QEMU = *((uint32_t *)ref_ptr + i);

  /*
 
  if((r[9]&0x00000001) != (eflags_QEMU&0x00000001)){//CF=0
		printf("diff:CF differ\n");
		printf("in QEMU, CF=%d\n", ((eflags_QEMU&0x00000001)==0)?(0):(1));
		printf("in NEMU, CF=%d\n", ((r[9]&0x00000001)==0)?(0):(1));
		flag = 1;
	}
	
	if((r[9]&0x00000040) != (eflags_QEMU&0x00000040)){//ZF=6
		printf("diff:ZF differ\n");
		printf("in QEMU, ZF=%d\n", ((eflags_QEMU&0x00000040)==0)?(0):(1));
		printf("in NEMU, ZF=%d\n", ((r[9]&0x00000040)==0)?(0):(1));
		flag = 1;
	}
	if((r[9]&0x00000080) != (eflags_QEMU&0x00000080)){//SF=7
		printf("diff:SF differ\n");
		printf("in QEMU, SF=%d\n", ((eflags_QEMU&0x00000080)==0)?(0):(1));
		printf("in NEMU, SF=%d\n", ((r[9]&0x00000080)==0)?(0):(1));
		flag = 1;
	}
  if((r[9]&0x00000800) != (eflags_QEMU&0x00000800)){//OF=11
		printf("diff:ZF differ\n");
		printf("in QEMU, OF=%d\n", ((eflags_QEMU&0x00000800)==0)?(0):(1));
		printf("in NEMU, OF=%d\n", ((r[9]&0x00000800)==0)?(0):(1));
		flag = 1;
	}

	*/
	//return true;
 	return !flag;
}

void isa_difftest_attach(void) {
}
