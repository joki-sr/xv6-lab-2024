//普通的find
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

static char name[DIRSIZ];
static char path[512];

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), 0, DIRSIZ-strlen(p));
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
        fprintf(2, "find: cannot open (%s)\n", path);
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
    // printf("fnm, name: %s,%s\n", fnm, name);
    
    //找到结果
    if(strcmp(fnm, name) == 0){
        // printf("!!\n");
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
        strcpy(name, argv[1]);
    }else if(argc == 3){ //find . a
        strcpy(path, argv[1]);
        strcpy(name, argv[2]);
    }
    find();
    exit(0);
}