#include "common.h"
#include <amdev.h>

void switch_pcb(int id);

size_t serial_write(const void *buf, size_t offset, size_t len) {
  _yield();
  char* p=(char*)buf;
  
  for(int i=0;i<len;i++)
    _putc(p[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};


size_t events_read(void *buf, size_t offset, size_t len) {
  _yield();
  int key=read_key();
  bool ifdown=false;
  if(key==_KEY_NONE){ //clk
    uint32_t time = uptime();
    sprintf(buf,"t %d\n",time);
  }
  else{
    //key down
    if(key&0x8000){
      ifdown=true;
    }
    key=key&0x7fff;
    switch(key){
        case _KEY_1:
          switch_pcb(1);
          break;
        case _KEY_2:
          switch_pcb(2);
          break;
        case _KEY_3:
          switch_pcb(3);
          break;
      }
    if(ifdown)
      sprintf(buf,"kd %s\n",keyname[key]);
    else
      sprintf(buf,"ku %s\n",keyname[key]);
  }

  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if(len+offset>strlen(dispinfo))
    len=strlen(dispinfo)-offset;
  strncpy(buf, dispinfo+offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  _yield();
  int x=(offset>>2)%screen_width();
  int y=(offset>>2)/screen_width();
  //printf("%x %d %d %d\n",buf,x,y,len);
  draw_rect((uint32_t*)buf,x,y,len>>2,1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
