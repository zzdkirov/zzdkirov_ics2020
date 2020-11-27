#include "fs.h"



typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char* pathname,int flags,int mode){
  printf("open\n");
  for(int i=0;i<NR_FILES;i++){
    if(!strcmp(file_table[i].name,pathname)){
      //open files, setting fp == 0;
      file_table[i].open_offset=0;
      return i;
    }

  }
  printf("No such file\n");
  assert(0);
  return -1;
}

int fs_read(int fd, const void *buf, size_t len){
  size_t ret=-1;
  Finfo* fp=&file_table[fd];

  if(fp->read==NULL){
    //judge if reach the end of the file
    if(fp->open_offset+len>fp->size){
      ret=fp->size-fp->open_offset;
    }
    else{
      ret=len;
    }
    ramdisk_read(buf,fp->disk_offset+fp->open_offset,ret);
  }
  else{
    ret=fp->read(buf,fp->open_offset,len);
  }
  file_table[fd].open_offset+=ret;
  printf("fuck\n");
  return ret;
}

int fs_close(int fd){
  return 0;
}

int fs_lseek(int fd,size_t offset,int whence){
  printf("seek\n");
  Finfo *fp = &file_table[fd];
  size_t res=-1;
  switch(whence){
    case SEEK_SET:
      if(offset>=0 && offset<=fp->size){
        fp->open_offset=offset;
        res=fp->open_offset;
      }
      break;
    case SEEK_CUR://cur+offset
      if((offset+fp->open_offset>=0)&&(offset+fp->open_offset<=fp->size)){
        fp->open_offset+=offset;
        res=fp->open_offset;
      }
      break;
    case SEEK_END://end+offset
      if((offset+fp->size>=0)&&(offset+fp->size<=fp->size)){
        fp->open_offset=fp->size+offset;
        res=fp->open_offset;
      }
      break;
    default:
      return -1;
  }
  return 0;
}

int fs_getopenoff(int fd){
  return file_table[fd].open_offset;
}

int fs_getdiskoff(int fd){
  return file_table[fd].disk_offset;
}

int fs_write(int fd, const void *buf, size_t len){
  size_t ret=-1;
  Finfo* fp=&file_table[fd];
  if(fp->write==NULL){
    //judge if reach the end of the file
    if(fp->open_offset+len>fp->size){
      ret=fp->size-fp->open_offset;
    }
    else{
      ret=len;
    }
    ramdisk_write(buf,fp->disk_offset+fp->open_offset,ret);
  }
  else{
    printf("");
    ret=fp->write(buf,fp->open_offset,len);
  }
  file_table[fd].open_offset+=ret;
  return ret;
}