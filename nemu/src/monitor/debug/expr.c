#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
static bool is_single_op(int index);
static uint32_t eval(int p,int q,bool *success);
uint32_t isa_reg_str2val(const char *s, bool *success);

enum {
  TK_NOTYPE = 256,TK_PLUS,TK_EQ,TK_NE,TK_MINUS,TK_MUL,TK_DIV,TK_LP,TK_RP,
  TK_INT,TK_HEX,TK_REG,TK_AND,TK_OR,TK_POS,TK_NEG,TK_POINTER

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_PLUS},         // plus, \\means transfer character due to the usage '+' in regular exp
  {"==", TK_EQ},         // equal
  {"\\-",TK_MINUS},
  {"\\*",TK_MUL},
  {"\\/",TK_DIV},
  {"\\(",TK_LP},
  {"\\)",TK_RP},
  {"!=",TK_NE},
  {"0[xX][0-9a-fA-F]+",TK_HEX},
  {"[0-9]+",TK_INT},
  {"\\$[a-zA-Z]{2,3}",TK_REG},  //no check register's name, checking it while using it
  {"&&",TK_AND},
  {"||",TK_OR},

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[1024] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          default:
            tokens[nr_token].type=rules[i].token_type;
          //It's boring finishing compilation principle.
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            nr_token++;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //handle single opreator
  for(int i=0;i<nr_token;i++){
    if(is_single_op(i)){
      switch(tokens[i].type){
        case TK_MUL:
          tokens[i].type=TK_POINTER;
          break;
        case TK_PLUS:
          tokens[i].type=TK_POS;
          break;
        case TK_MINUS:
          tokens[i].type=TK_NEG;
          break;
      }
    }
    
  }

  return eval(0,nr_token-1,success);


}

//only support positive & negetive & pointer
static bool is_single_op(int index){
  if(index==0)
    return true;
  if(tokens[index].type!=TK_MUL
    ||tokens[index].type!=TK_PLUS
    ||tokens[index].type!=TK_MINUS)
    return false;
  switch(tokens[index-1].type){
    case TK_PLUS:
    case TK_MINUS:
    case TK_MUL:
    case TK_DIV:
    case TK_EQ:
    case TK_NE:
    case TK_AND:
    case TK_OR:
    case TK_LP:
      return true;
    default:
      return false;
  }
}

//judge if now token is op character
static bool is_opchar(int index)
{
  switch(tokens[index].type)
  {
    case TK_NOTYPE:
    case TK_LP:
    case TK_RP:
    case TK_REG:
    case TK_INT:
    case TK_HEX:
      return false;
    default:
      return true;
  }
}

//different to poland exp, the higher opchar's prior, the lower its level (for this method)
static int opchar_level(int index)
{
  switch(tokens[index].type)
  {
    case TK_POINTER:
    case TK_POS:
    case TK_NEG:
      return 1;
    case TK_MUL:
    case TK_DIV:
      return 2;
    case TK_PLUS:
    case TK_MINUS:
      return 3;
    case TK_EQ:
    case TK_NE:
      return 5;
    case TK_AND:
      return 6;
    case TK_OR:
      return 7;
    
  }
  return -1;
}
static bool check_parentheses(int p,int q){
  if(tokens[p].type!=TK_LP||tokens[q].type!=TK_RP)
    return false;
  int pnum=0;
  for(int i=p+1;i<q;i++){
    if(tokens[i].type==TK_LP)
      pnum++;
    else if(tokens[i].type==TK_RP){
      pnum--;
      if(pnum<0)
        return false;
    }
  }
  return pnum==0;
}

static uint32_t eval(int p,int q,bool *success){
  unsigned int value;
  if(p>q){
    printf("Bad expression with %d\n",p);
    *success=false;
    return 0;
  }
  else if(p==q){
    switch(tokens[p].type){
      case TK_INT:
        sscanf(tokens[p].str,"%u",&value);
        break;
      case TK_HEX:
        sscanf(tokens[p].str,"%x",&value);
        break;
      case TK_REG:
        value=isa_reg_str2val(tokens[p].str,success);
        break;
      default:
        *success=false;
        break;
    }
    return value;
  }
  else if (check_parentheses(p, q)){
    return eval(p+1,q-1,success);
  }
  else{
    //get main opchar, main op char is caculated latest
    int pnum=0; //'()'num, express in () is not included in main opchar
    int pos=p;  //now main op position
    int level=-1;  //now main op level, temp var

    for(int i=p;i<=q;i++){
      if(tokens[i].type==TK_LP){
        pnum++;
      }
      else if(tokens[i].type==TK_RP){
        pnum--;
      }
      else if(is_opchar(i)==false){
        continue;
      }

      if(pnum>0){
        continue;
      }
      else if(pnum<0){  //pnum<0 means an RP is eralier than LP
        *success=false;
        return 0;
      }
      else{
        if(opchar_level(i)>=level){
          level=opchar_level(i);
          pos=i;
        } //low level opchar should caculate later
          //if level is equal, the later opchar is should caculate later
      }
    }
    switch(tokens[pos].type)
    {
      case TK_POINTER:
        return pmem[eval(pos+1,q,success)];
      case TK_POS:
        return eval(pos+1,q,success);
      case TK_NEG:
        return -eval(pos+1,q,success);
      default:
      {
        uint32_t val1=eval(p,pos-1,success);
        uint32_t val2=eval(pos+1,q,success);

        switch (tokens[pos].type)
        {
          case TK_PLUS:
            return val1+val2;
          case TK_MINUS:
            return val1-val2;
          case TK_MUL:
            return val1*val2;
          case TK_DIV:
            if(val2==0){
              printf("Divide Zero!");
              *success=false;
              return 0;
            }
            return val1/val2;
          case TK_EQ:
            return val1==val2;
          case TK_NE:
            return val1!=val2;
          case TK_AND:
            return val1&&val2;
          case TK_OR:
            return val1||val2;
          default:
            printf("Invalid main opreation character\n");
            *success=0;
            return 0;

        }
      }
    }
    
    
  }
}