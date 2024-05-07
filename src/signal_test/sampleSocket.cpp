#include <stdio.h>
#include "self_string.h"
#include "add_float.h"
#include "add_int.h"
#include "sub_float.h"
#include "sub_int.h"
#include "file_test.h"

#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

int fd = -1;

void sig_process_int(int signo){
    printf("\n receive signo %d \n",signo);
    close(fd);
    exit(0);
}

void sig_process_pipe(int signo){
    printf("\n receive signo %d \n",signo);
    close(fd);
    exit(0);
}

/**
 * Test file.h with file.cpp
*/
void test(){
    print_string();
    printf("10+20 = %d \n",add_int(10,20));
    printf("10+20 = %ld \n",add_int_l(10,20));
    printf("10+20 = %f \n",add_float(10,20));
    printf("10+20 = %f \n",add_float_d(10,20));

    printf("10-20 = %d \n",sub_int(10,20));
    printf("10-20 = %ld \n",sub_int_l(10,20));
    printf("10-20 = %f \n",sub_float(10,20));
    printf("10-20 = %f \n",sub_float_d(10,20));

    test_file("./test.txt");
}

void test_signal(){
    signal(SIGINT,sig_process_int);
    signal(SIGPIPE,sig_process_pipe);
}

int test_main(int argc,char *argv[]){
    test_signal();
    fd = open("/dev/stdout",O_WRONLY);
    char c = '.';
    while(1){
        write(fd,&c,sizeof(c));
        sleep(1);
    }
    close(fd);
    return 0;
}