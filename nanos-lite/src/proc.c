#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
int currentpcbid=1;

void context_kload(PCB *pcb, void *entry);
void naive_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void switch_pcb(int id){
  currentpcbid=id;
  Log("switch proc %d\n",id);
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

/*
这里由于虚拟机性能问题，无法验证这些实现的正确性，因此不一定确保通过数字键切换进程有效
因此，最好还是改为直接多个进程运行
在我的电脑上，同时运行多个pal是非常不现实的
!!!!!!!!!!!!!!!!通过打开宏MULTIPAL来检测通过按键切换进程
*/
//#define MULTIPAL
void init_proc() {
  context_uload(&pcb[0],"/bin/hello");
  context_uload(&pcb[1],"/bin/pal");
  #ifdef MULTIPAL
    context_uload(&pcb[1],"/bin/pal");
    context_uload(&pcb[1],"/bin/pal");
  #endif
  switch_boot_pcb();
  Log("Initializing processes...");

  // load program here, for pa3
  //naive_uload(&pcb[1], "/bin/init");

}
int counter_schedule=0;
_Context* schedule(_Context *prev) {
  current->cp=prev;
  if(counter_schedule++ >200){
    current=&pcb[0];
    counter_schedule=0;
  }
  else
  {
    #ifdef MULTIPAL
      current=&pcb[currentpcbid];
    #endif

    #ifndef MULTIPAL
      current=&pcb[1];
    #endif
  }
  
  //current= (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
