#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#ifndef PRIME_SUM
#define PRIME_SUM 10
#endif

// p (w)---le---(r) p (w)---ri ---(r)p
//p[0] :read
void proc(int le[2]){
    int pid = fork();
    int ri[2];
    pipe(ri);
    int pri = 0;
    static int cnt = 0;  // 计数器要是 static 的，以保持值
    // cnt++;
    // read(le[])
    // printf("cnt: %d\n", cnt);

    if(pid == 0){
        proc(ri);
    }else{
// printf("222222222\n");
        close(ri[0]);//关闭右侧r
        close(le[1]);//关闭左侧写
        if (read(le[0], &pri, sizeof(pri)) > 0){ //读第一个传进来的，是质数
            cnt++;
            printf("prime: %d cnt: %d\n", pri, cnt);
        }

        int num;
        while(read(le[0], &num, sizeof(num))){
            if (num % pri != 0) {  // 如果不是质数的倍数
                write(ri[1], &num, sizeof(num));  // 传递给下一个进程
            }
        }
        close(ri[1]);
        close(le[0]);

    }

}

int main(int argc, char *argv[]){
    //generate
    int p[2];
    pipe(p);
    
    if(fork() == 0){
        proc(p);
    }else{
        close(p[0]);
        for(int i=2;i<=PRIME_SUM;i++){
            // printf("s%d\n", i);
            write(p[1], &i, sizeof(i));
        }
        close(p[1]);
    }
    

}