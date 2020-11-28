#include "proc.h"
#include <elf.h>
#include "fs.h"

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd,void *buf,size_t len);
int fs_close(int fd);
int fs_lseek(int fd,size_t offset,int whence);
int fs_getopenoff(int fd);
int fs_getdiskoff(int fd);



static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elfheader; //excute file header, locate start address
  Elf_Phdr prgmheader;  //program loader header, locate every segment
  int fd=fs_open(filename,0,0);
  int phoffset=0;

  if(fd!=-1){
    fs_read(fd,&elfheader,sizeof(Elf_Ehdr));
    fs_lseek(fd,elfheader.e_phoff,SEEK_SET);
    for(int i=0;i<elfheader.e_phnum;i++){
      fs_read(fd,&prgmheader,sizeof(Elf_Phdr));
      phoffset=fs_getopenoff(fd);
      if(prgmheader.p_type==PT_LOAD){
        void *vaddr=(void*)prgmheader.p_vaddr;
        fs_lseek(fd,prgmheader.p_offset,SEEK_SET);
        fs_read(fd,vaddr,prgmheader.p_filesz);
        //ramdisk_read((void*)buffer,prgmheader.p_offset+fileoffset,prgmheader.p_filesz);
        //memcpy(vaddr,(void*)buffer,prgmheader.p_filesz);
        memset((void*)(prgmheader.p_vaddr+prgmheader.p_filesz),0,prgmheader.p_memsz-prgmheader.p_filesz);
      }
      fs_lseek(fd,phoffset,SEEK_SET);
    }
    
  }
  fs_close(fd);
  
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
