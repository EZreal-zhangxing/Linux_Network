#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void test_file(char * path){
    int fd = open(path,O_CREAT|O_EXCL|O_RDWR,S_IRWXU);
    if(fd != -1){
        printf("open file %s success !\n",path);
    }else{
        printf("open file %s failed ! because it's existed \n",path);
        printf("retry...\n");
        fd = open(path,O_APPEND|O_WRONLY);
        if(fd != -1){
            printf("reopen file success ! fd %d\n",fd);
            char lines[10] = "hello";
            auto size = write(fd,lines,5);
            printf("write bytes %ld\n",size);
        }
    }
    close(fd);
}