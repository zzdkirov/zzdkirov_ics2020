#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elfheader; //excute file header, locate start address
  Elf_Phdr prgmheader;  //program loader header, locate every segment
  ramdisk_read(&elfheader,0,sizeof(Elf_Ehdr));

  size_t phoffset=elfheader.e_phoff;

  for(uint16_t i=0;i<elfheader.e_phnum;i++){
    ramdisk_read(&prgmheader,phoffset,sizeof(Elf_Phdr));
    if(prgmheader.p_type==PT_LOAD){
      void *vaddr = prgmheader.p_vaddr;
      ramdisk_read(vaddr,prgmheader.p_offset,prgmheader.p_memsz);
      memset(vaddr,'\0',prgmheader.p_memsz-prgmheader.p_filesz);
    }
    phoffset+=sizeof(Elf_Phdr);
  }


  return elfheader.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
