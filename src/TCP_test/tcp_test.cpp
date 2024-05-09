#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h> // 地址结构所在头文件
#include <string.h> // bzero 所在头文件

#include<arpa/inet.h> // 地址转换所在头文件

#include<stdio.h>
#include<errno.h>

#include<pthread.h>
#include<stdlib.h>

#include<signal.h>

#include<string.h>
#define PORT 10000
#define LISTEN_LEN 10
static int ss,sc;
static int is_alive = 1;   
void signal_process_tcp(int sig){
    is_alive = 0;
    close(ss);
    close(sc);
    exit(0);
}

 
void *tcp_service(void *){
    int res = 0;
    ss = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in serviceAddr;

    bzero(&serviceAddr,sizeof(sockaddr_in));
    serviceAddr.sin_family = AF_INET;
    serviceAddr.sin_addr.s_addr = htonl(INADDR_ANY); // long 8Byte short 2Byte
    serviceAddr.sin_port = htons(PORT);

    res = bind(ss,(sockaddr *)&serviceAddr,sizeof(sockaddr_in));
    if(res != 0){
        printf("bind failed error : %d ,%s \n",errno,strerror(errno));
    }

    res = listen(ss,LISTEN_LEN);
    if(res != 0){
        printf("listen failed error : %d ,%s \n",errno,strerror(errno));
    }
    char buffer[1024] = "";
    while(is_alive){
        sockaddr_in clientAddr;
        socklen_t socketlen = sizeof(sockaddr_in);
        bzero(&clientAddr,socketlen);
        int ssc = accept(ss,(sockaddr *)&clientAddr,&socketlen);
        printf("[%s] connect success \n",inet_ntoa(clientAddr.sin_addr));

        res = read(ssc,buffer,1024);
        if(res < 0){
            printf("recive failed %d %s \n",errno,strerror(errno));
        }else{
            printf("recive: %s \n",buffer);
        }
        close(ssc);
    }
    close(ss);
    return 0;
}

void *tcp_client(void * index){
    int res = 0;
    sc = socket(AF_INET,SOCK_STREAM,0);
    char buffer[1024]="";
    sprintf(buffer,"%s %d","Hello from client",*(int *)(index));
    sockaddr_in serviceAddr;
    bzero(&serviceAddr,sizeof(sockaddr_in));
    serviceAddr.sin_family = AF_INET;
    serviceAddr.sin_addr.s_addr = htonl(inet_network("127.0.0.1"));
    serviceAddr.sin_port = htons(PORT);
    res = connect(sc,(sockaddr *)&serviceAddr,sizeof(sockaddr_in));
    if(res != 0){
        printf("connect service failed: %s \n",strerror(errno));
    }else{
        write(sc,buffer,1024);
    }
    close(sc);
    return 0;
}

void test_tcp_connect(){
    signal(SIGINT,signal_process_tcp);
    pthread_t service,client[LISTEN_LEN];

    pthread_create(&service,NULL,tcp_service,NULL);
    for(int i=0;i<LISTEN_LEN;i++){
        pthread_create(client+i,NULL,tcp_client,&i);
        // pthread_join(client[i],NULL);
        sleep(1);
    }
    pthread_join(service,NULL);

    printf("test tcp connect exit \n");
}