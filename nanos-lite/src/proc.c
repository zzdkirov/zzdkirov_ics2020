#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
int currentpcbid=0;

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

void init_proc() {
  context_uload(&pcb[0],"/bin/hello");
  context_uload(&pcb[1],"/bin/pal");
  context_uload(&pcb[1],"/bin/pal");
  context_uload(&pcb[1],"/bin/pal");
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
    current=&pcb[currentpcbid];
  }
  
  //current= (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
