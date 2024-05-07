#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<string.h>
int pipe_test(int argc,char * argv[]){

    printf("parent pid %d \n",getpid());
    int filedes[2];
    if(pipe(filedes) < 0){
        printf("create pipe failed \n");
    }
    if(fork() == 0){
        // 子进程
        printf("child pid %d \n",getpid());
        // char read_temp[20];
        // read(filedes[0],read_temp,20);
        // printf("children read from filedes 0: %s\n",read_temp);

        char write_temp[20] = "children send";
        write(filedes[1],write_temp,20);
        printf("children write to filedes 1: %s\n",write_temp);
        
        // close(filedes[0]);
        close(filedes[1]);
    }else{
        // char temp[20] = "parent send this";
        // write(filedes[1],temp,20);
        // printf("parent write to filedes 1: %s\n",temp);

        char temp1[20];
        read(filedes[0],temp1,20);
        printf("parent read from filedes 0: %s\n",temp1);

        close(filedes[0]);
        // close(filedes[1]);
    }
 
    return 0;
}

#define WRITE_BYTES 1024*128*2
int pipe_write_read(){
    int filedes[2];
    if(pipe(filedes) < 0){
        printf("create pipe failed \n");
    }

    if(fork() == 0){
        // 子进程
        printf("child pid %d \n",getpid());
        char write_temp[WRITE_BYTES] = "";
        int write_bytes = write(filedes[1],write_temp,WRITE_BYTES);
        printf("children write bytes: %d\n",write_bytes);
        
        close(filedes[1]);
    }else{
        char temp1[1024*8];
        int read_bytes = 0;
        int sum = 0;
        do{
            read_bytes = read(filedes[0],temp1,1024*8);
            sum += read_bytes;
            printf("parent read bytes: [%d / %5d]\n",read_bytes,sum);
        }
        while(sum < WRITE_BYTES);
        close(filedes[0]);
    }
 
    return 0;
}