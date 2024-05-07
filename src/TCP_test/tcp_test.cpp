#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h> // 地址结构所在头文件
#include <string.h> // bzero 所在头文件

#include<arpa/inet.h> // 地址转换所在头文件

#include<stdio.h>
#include<errno.h>

#define PORT 10000

void signal_process_tcp(int sig){
    is_alive = 0;
}

static int is_alive = 1;    
void tcp_service(void *){
    int res = 0;
    int ss = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in serviceAddr;

    bzero(&serviceAddr,sizeof(sockaddr_in));
    serviceAddr.sin_family = AF_INET;
    serviceAddr.sin_addr.s_addr = htonl(INADDR_ANY); // long 8Byte short 2Byte
    serviceAddr.sin_port = htons(PORT);

    res = bind(ss,(sockaddr *)&serviceAddr,sizeof(sockaddr_in));
    if(res != 0){
        printf("bind failed error : %d ,%s \n",errno,strerror(errno));
    }

    res = listen(ss,10);
    if(res != 0){
        printf("listen failed error : %d ,%s \n",errno,strerror(errno));
    }
    char buffer[1024];
    while(is_alive){
        sockaddr_in clientAddr;
        socklen_t socketlen = sizeof(sockaddr_in);
        res = accept(ss,(sockaddr *)&clientAddr,&socketlen);
        printf("[%s] connect \n",inet_ntoa(clientAddr.sin_addr));

        read(ss,buffer,1024);
        printf("recive: %s \n",buffer);
    }
    close(ss);
}

void tcp_client(void *){
    
}