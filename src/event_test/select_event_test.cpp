#include<unistd.h>
#include<sys/types.h>
#include<sys/times.h>
#include<stdio.h>


int select_event_test(int argc,char * argv[]){
    fd_set fs;
    FD_ZERO(&fs);
    FD_SET(0,&fs);

    fd_set wfs;
    FD_ZERO(&wfs);
    FD_SET(1,&wfs);
    timeval wait;
    wait.tv_sec = 10;
    wait.tv_usec = 0;
    printf("write sth\n");
    int res = select(2,&fs,&wfs,NULL,&wait);

    if(res == 0){
        printf("method select is timeout \n");
    }else if(res >0){
        printf("select is ready \n");
        char input[10];
        int readr = read(0,input,9);
        input[9] = '\0';    
        printf("input : %s \n",input);
    }else{
        printf("method select is error \n");
    }

    return 0;
}