#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];

int lth=0;

int genlevel=0; //recurrence level

//generate [0,range) random number
static int gen_randnum(int range){
  return rand()%range;
}

static void bufaddr(char c){
  buf[lth++]=c;
  buf[lth]='\0';
}



static void gen_bin_op()
{
  switch(gen_randnum(8)){
    case 0:
      bufaddr('+');
      break;
    case 1:
      bufaddr('-');
      break;
    case 2:
      bufaddr('*');
      break;
    case 3:
      bufaddr('/');
      break;
    case 4:
      bufaddr('&');
      bufaddr('&');
      break;
    case 5:
      bufaddr('|');
      bufaddr('|');
      break;
    case 6:
      bufaddr('=');
      bufaddr('=');
      break;
    case 7:
      bufaddr('!');
      bufaddr('=');
      break;

  }
}

static void gen_num(){
  int range=gen_randnum(4)+1;
  if(range!=1){
    bufaddr('0'+gen_randnum(9)+1);  //first digit can't be zero
    for(int i=1;i<range;i++)
    {
      bufaddr('0'+gen_randnum(10));
    }
  }
  else{
    bufaddr('0'+gen_randnum(10));
  }
  
}

static void gen_rand_expr() {

  //exp include number and op(not must choice)
  //3 methods:
  //1.generate a number
  //2.genereate a exp op exp
  //3.genereate a (exp) (now treat () as a normal opreator)
  if(genlevel>=10)
    return;
  else 
    genlevel++;

    switch(gen_randnum(3)){
      case 0:
        gen_num();
        break;
      case 1:
        gen_rand_expr();
        gen_bin_op();
        gen_rand_expr();
        break;
      case 2:
        bufaddr('(');
        gen_rand_expr();
        bufaddr(')');
    }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
