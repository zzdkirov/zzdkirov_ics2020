#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display();
void display_watchpoint();
void exec_once();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_info(char *args);
static int cmd_x(char * args);
static int cmd_p(char* args);
static int cmd_w(char* args);
static int cmd_d(char* args);
static int cmd_si(char *args)

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "info", "Display register status with option 'r' and watch points with option 'w'", cmd_info },
  { "x", "with option --N EXPR, display N words from address EXPR in memory ", cmd_x },
  { "p", "with option --EXPR, caculate the expression",cmd_p },
  { "w", "w EXPR setting a watchpoint",cmd_w},
  { "d", "d N delete a watchpoint",cmd_d},
  { "si","si [N] execute instructions for N line then stop,default step by step", cmd_si},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_info(char *args){
  char* arg = strtok(NULL," ");
  if(arg==NULL){
    printf("No options are given: input info [r/w]\n");
  }
  else {
    if(!strcmp(arg,"r")){
      isa_reg_display();
    }
    else if(!strcmp(arg,"w")){
      display_watchpoint();
    }
    else{
      printf("Unknown command '%s'\n", arg);
    }
  }
  return 0;
}

static int cmd_x(char *args){
  char* arg = strtok(NULL," ");
  int num,addr;
  bool success;
  if(arg==NULL){
    printf("No options are given: input x N addr\n");
  }
  else{
    if(sscanf(arg,"%u",&num)!=1){
      printf("Num must be a positive integer\n");
    }
    else{
      arg=strtok(NULL," ");
      if(arg==NULL){
        printf("No options are given: input x N addr\n");
      }
      else{
        addr=expr(arg,&success);
        if(!success){
          printf("Addr Expression caculate fail\n");
        }
        else{
          for(int i=0;i<num;i++){
						printf("0x%08x: 0x",addr+4*i);
						for(int j=0;j<4;j++){
								printf("%02x ",*(unsigned char *)(&pmem[addr+4*i+j]));
						}
						printf("\n");
				}
        }
      }
      
    }
    
  }

  return 0;
}

static int cmd_p(char *args){
  uint32_t value;
  bool success=1;

  value=expr(args,&success);

  if(!success){
    printf("invaild expression\n");
    return 0;
  }
  else{
    printf("%d 0x%x\n",value,value);
    return 0;
  }

}

static int cmd_d(char* args){
  int num;
  sscanf(args,"%d",&num);
  if(!delete_wp(num)){
    printf("No such wp member\n");
  }
}


static int cmd_w(char* args){
  bool success=true;
  uint32_t val=expr(args,&success);
  if(!success){
    printf("invaild expression\n");
    return 0;
  }
  WP* p = new_wp();
  strcpy(p->exp,args);
  p->value = val;
  return 0;

}

static int cmd_si(char* args){
  int n;
  if(args==NULL){
    sscanf(args,"%d",&n);
  }
  else{
    n=1;
  }

  for(int i=0;i<n;i++){
    exec_once();
  }
  
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
