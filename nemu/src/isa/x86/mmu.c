#include "nemu.h"
#include "isa/mmu.h"

static paddr_t page_translate(vaddr_t vaddr){
  //page dir addr = vaddr[20:32]*sizeof(PDE)+cr3.pagedirbase<<12
  CR3 cr3;
  PDE pde; //page dir entry
  PTE pte;  //page table entry
  paddr_t paddr;

  cr3.val=cpu.cr3;

  pde.val=paddr_read(((vaddr>>22)&(0x3ff))*sizeof(PDE)+(cr3.page_directory_base<<12),sizeof(PDE));
  printf("%x\n",pde.val);
  assert(pde.present==1);
  pte.val=paddr_read((pde.page_frame<<12) + ((vaddr>>12)&(0x3ff)) * sizeof(PTE), sizeof(PTE));
  assert(pte.present==1);
  paddr=(pte.page_frame<<12)|(vaddr & PAGE_MASK);
  return paddr;
}

//WARNING: page is not completed in nemu or pa3, so if we use page to run pa3, error must f**k my brain
//maybe len==1 ==2 ==3 ==4

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  CR0 cr0;
  cr0.val=cpu.cr0;
  if(cr0.paging){
    if((addr & PAGE_MASK) + len > PAGE_SIZE){
      int len1,len2;
      paddr_t paddr1,paddr2;
      uint32_t val1,val2;
      len1=PAGE_SIZE-(addr & PAGE_MASK);
      len2=len-len1;
      paddr1=page_translate(addr);
      paddr2=page_translate(addr+len1);
      val1=paddr_read(paddr1,len1);
      val2=paddr_read(paddr2,len2);
      //val2 left shift 8,16,24 or val1
      return (val2<<(len1<<3))|val1; 
    }
    else{
      paddr_t paddr = page_translate(addr);
      return paddr_read(paddr, len);
    }
  }
  else{
    return paddr_read(addr,len);
  }
  
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  CR0 cr0;
  cr0.val=cpu.cr0;
  if(cr0.paging){
    if((addr & PAGE_MASK) + len > PAGE_SIZE){
      int len1,len2;
      paddr_t paddr1,paddr2;
      len1=PAGE_SIZE-(addr & PAGE_MASK);
      len2=len-len1;
      paddr1=page_translate(addr);
      paddr2=page_translate(addr+len1);
      paddr_write(paddr1,data,len1);
      paddr_write(paddr2,data>>(len1<<3),len2);
    }else{
      paddr_t paddr = page_translate(addr);
      paddr_write(paddr, data, len);
    }
  }
  else{
    paddr_write(addr,data,len);
  }
}
