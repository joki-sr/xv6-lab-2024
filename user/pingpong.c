#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


// fd 0 : stdin
// fd 1: stdout
// fd 2: stderr

// 父进程向子进程发送一个字节；
// 子进程输出 "<pid>: received ping"，其中 <pid> 是子进程的进程 ID，
// 随后将该字节写回给父进程，并退出；
// 父进程从子进程读取该字节，输出 "<pid>: received pong"，并退出。

int main(int argc, char *argv[]){
    int p[2];
    pipe(p);
    char buf[1] ={'c'};


    if( fork() == 0){
        // int pid = getpid();
        // printf("kid, pid=%d\n", pid);        
        close(0);
        dup(p[0]); //stdin
// printf("kid\n");
        if( read(0 ,buf, 1 ) ){
// printf("kid read\n");
            printf("%d: received ping\n", getpid());
            close(p[0]);
            close(p[1]);
// printf("kid end\n");
        }      
    }else{
        int pid = getpid();
        // printf("par, pid=%d\n", pid);
        close(p[0]);
        write(p[1] , buf, 1);
// printf("parent write\n");
        close(p[1]);
// printf("parent wait\n");
        wait((int *)0); //暂停，知道子进程结束
        printf("%d: received pong\n", pid);
    }
    exit(0);
}