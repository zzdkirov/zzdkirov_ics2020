#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t lth=0;
  while(s[lth]!='\0')
    lth++;
  return lth;
}

char *strcpy(char* dst,const char* src) {
  size_t i = 0;
	while(src[i] != '\0'){
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
	for(i=0;i<n&&src[i]!='\0';i++){
		dst[i] = src[i];
	}
	dst[i]='\0';
	return dst;
}

char* strcat(char* dst, const char* src) {
  size_t i=strlen(dst);
	size_t j=0;
	while(src[j]!='\0'){
		dst[i]=src[j];
		i++;
		j++;
	}
	dst[i]=src[j];
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i=0;
	while(s1[i]==s2[i]&&s1[i]!='\0'){
		i++;
	}
	if(s1[i]==s2[i]){
		return 0;
	}else if(s1[i]<s2[i]){
		return -1;
	}else{
		return 1;
	}
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t i;
	for(i=0; i<n;i++){
		if(s1[i] < s2[i]){
			return -1;
		}else if(s1[i] > s2[i]){
			return 1;
		}
	}

  return 0;
}

void* memset(void* v,int c,size_t n) {
  size_t i;
	(unsigned char) *p=(unsigned char*)v; 
	for(i=0;i<n;i++){
		p[i]=c;
	}
  return p;
}

void* memcpy(void* out, const void* in, size_t n) {
  size_t i;
	unsigned char *pout=(unsigned char*)out;
  unsigned char *pin=(unsigned char*)in;
	for(i=0;i<n;i++){
		pout[i]=pin[i];
	}
  return pout;
}

int memcmp(const void* s1, const void* s2, size_t n){
  size_t i;
	unsigned char *p1=(unsigned char*)s1;
	unsigned char *p2=(unsigned char*)s2;
	for(i=0;i<n;i++){
		if(p1[i]<p2[i]){
			return -1;
		}else if(p1[i]>p2[i]){
			return 1;
		}else{
			continue;
		}
	}
  return 0;
}

#endif
