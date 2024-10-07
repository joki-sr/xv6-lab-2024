// 支持正则表达式
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#ifndef PTNSZ
#define PTNSZ 1024
#endif

static char pattern[PTNSZ];
static char path[512];

int match(char*, char*);
char*
fmtname(char *path)
{
  static char buf[PTNSZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= PTNSZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), 0, PTNSZ-strlen(p));
  return buf;
}

void find(void){
    // printf("find path: %s\n", path);
    char *p;
    int fd;
    struct dirent de;
    struct stat st;

    int isFile = 0;

    if( (fd = open(path, O_RDONLY)) < 0){
        isFile = 1;
        // fprintf(2, "find: cannot open (%s)\n", path);
    }
    else{
        // printf("open path: %s\n", path);
    }
    
    if( fstat(fd, &st) < 0){
        isFile = 1;
        // fprintf(2, "find: cannot stat %s\n", path);
    }
    else{
        // close(fd);
    }

    char *fnm = fmtname(path);
    // printf("fnm, name: %s,%s\n", fnm, pattern);
    
    //找到结果
    // if(strcmp(fnm, name) == 0){
    // printf("cmp: fnm:%s pattern:%s\n", fnm, pattern);
    if(match(pattern, fnm)){
        // printf("!!\n");
        // printf("%s match %s\n", pattern, fnm);
        printf("%s\n", path);
    }
    if(isFile){
        close(fd);
        return;
    }

    switch (st.type){
        case T_DIR:
            if((strlen(path) + 1 + DIRSIZ + 1) > sizeof(path)){
                // printf("find: path too long\n");
                return;
            }
            p = path + strlen(path);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                // printf("de.name: %s\n", de.name);
                p[DIRSIZ] = 0;
                find();
            }
            close(fd);
            break;
        default:
            close(fd);
            break;
    }
}

int main(int argc, char *argv[]){
    if(argc == 2){ //find a
        strcpy(path, ".");
        strcpy(pattern, argv[1]);
    }else if(argc == 3){ //find . a
        strcpy(path, argv[1]);
        strcpy(pattern, argv[2]);
    }
    find();
    exit(0);
}


// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

/*
mkdir abc
echo hello > a/abc
echo hello > abc
find . abc


find . b | xargs grep hello

find . b

*/