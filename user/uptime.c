#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    printf("%u ticks since system start\n", uptime());
    exit(0);
}