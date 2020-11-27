#include "common.h"
#include "syscall.h"

int fs_write(int fd, const void *buf, size_t len);
int fs_read(int fd,void* buf,size_t len);
int fs_close(int fd);
int fs_open(const char *pathname, int flags, int mode);
int fs_lseek(int fd,size_t offset,int whence);

int sys_yield(){
  _yield();
  return 0;
}
void sys_exit(uintptr_t arg){
  _halt(arg);
}

size_t sys_write(int fd,const void *buf,size_t count){
  char* p=(char*)buf;
  if(fd==1||fd==2){
    for(int i=0;i<count;i++){
      _putc(p[i]);
    }
  }
  return count;
}



_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  //printf("%d\n",a[0]);
  switch (a[0]) {
    case SYS_yield: c->GPRx=sys_yield();break;
    case SYS_exit: sys_exit(a[1]);break;
    case SYS_write: c->GPRx=fs_write(a[1],(void*)a[2],a[3]);break;
    case SYS_read: c->GPRx=fs_read(a[1],(void*)a[2],a[3]);break;
    case SYS_close: c->GPRx=fs_close(a[1]);break;
    case SYS_open: c->GPRx=fs_open((char*)a[1],a[2],a[3]);break;
    case SYS_lseek: c->GPRx=fs_lseek(a[1],a[2],a[3]);break;
    case SYS_brk:c->GPRx=0;break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

