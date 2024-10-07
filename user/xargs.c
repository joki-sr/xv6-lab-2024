#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"  // For MAXARG

// static void putc(int fd, char c);

int main(int argc, char*argv[]){
    if(argc < 2){
        fprintf(2, "Usage: xargs <command>\n");
        exit(1);
    }

    char buf[512];
    char *args[MAXARG];
    for(int i=0;i<argc - 1;i++){
        args[i] = argv[i+1];
    }
    // char ch = 'a';printf("te: %c\n", ch);
    char ch;
    int len = 0;
    memset(buf, 0, sizeof(buf));
    while( read(0, &ch, sizeof(ch)) == sizeof(ch) ){
        if(ch == '\n'){
            buf[len] = 0;
            len = 0;
            // printf("buf: %s\n", buf);
            args[argc -1 ] = buf;
            // printf("args[%d] = %s\n", argc - 1, buf);
            args[argc] = 0;

            // //输出执行的指令
            // for(int i=0;i<argc;i++){
            //     printf("%s ",args[i]);
            // }
            // printf("\n");

            if(fork() == 0){
                exec(args[0], args);

                fprintf(2, "xargs: exec %s failed\n", args[0]);
            }else{
                wait(0);
                memset(buf, 0, sizeof(buf));
            }
        }else{
            buf[len++] = ch;
            // printf("t buf: %s\n", buf);
        }
    }
    exit(0);
}


/*
$ make qemu
...
init: starting sh
$ sh < xargstest.sh
$ $ $ $ $ $ hello
hello
hello
$ $
*/

/*
sh < xargstest.sh
*/