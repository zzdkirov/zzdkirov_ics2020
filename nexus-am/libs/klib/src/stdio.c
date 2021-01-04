#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


int printf(const char *fmt, ...) {
  char buf[2560];
	va_list args;
	int n;
	va_start(args, fmt);
	n = vsprintf(buf, fmt, args);
  va_end(args);
  for(int i=0 ; buf[i]!='\0' ; i++ )
  {
    _putc(buf[i]);
  }
	
	return n;
}

char* itoa2(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//index table
    unsigned unum;
    int i=0,j,k;
 
    if(radix==10&&num<0)
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;
 

    do
    {
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
 
    }while(unum);
 
    str[i]='\0';
 
    if(str[0]=='-') k=1;
    else k=0;
 
    char temp;
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
 
    return str;
 
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    char* p;
    char tmp[256];
    va_list p_next_arg = ap;
    int tempi;
    char* tempc;
    int i=0;

    for (p = out;*fmt;fmt++)
    {
      i=0;
        if (*fmt != '%')
        {
            *p++ = *fmt;
            continue;
        }
 
        fmt++;
 
        switch (*fmt)
        {
            case 'd':
                tempi=va_arg(p_next_arg,int);
                itoa2(tempi,tmp,10);
                while(tmp[i]!='\0'){
                  *p++=tmp[i++];
                }
                break;
            case 'x':case 'p':
                tempi=va_arg(p_next_arg,int);
                itoa2(tempi,tmp,16);
                while(tmp[i]!='\0'){
                  *p++=tmp[i++];
                }
                break;
            case 's':
                tempc=va_arg(p_next_arg,char*);
                while(tempc[i]!='\0'){
                  *p++=tempc[i++];
                }
                break;
            default:
                break;
        }
    }
    *p='\0';
   
    return (p - out);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list para;
  int n;
	va_start(para, fmt);
	n = vsprintf(out, fmt, para);

	va_end(para);
  


  return n;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
