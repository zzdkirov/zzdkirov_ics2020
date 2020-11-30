#include <am.h>
#include <amdev.h>
#include <nemu.h>

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      uint32_t temp=inl(SCREEN_ADDR);
      info->width = (temp&0xffff0000)>>16;
      info->height = (temp&0xffff);
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      int x=ctl->x;
      int y=ctl->y;
      int w=ctl->w;
      int h=ctl->h;
      uint32_t* ppx=ctl->pixels;
      //400*300 px picture
      uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;

      //unreveal error: color is not suit for screen
      for(int i=0;i<h;i++){
        for(int j=0;j<w;j++){
          fb[(y+i)*400+x+j]=ppx[i*w+j];
        }
      }


      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
