#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#include<netinet/in.h>

#include<sys/uio.h>
#include<errno.h>

#include<sys/select.h>
#include<sys/time.h>

#include<pthread.h>
#include<signal.h>
#define PORT 8888

int ss,sc;
static int is_alive = 1;
void process_sigint(int sig){
    is_alive = 0;
    close(ss);
    close(sc);
    exit(0);
}

void select_read(fd_set * fds,int maxfd){
    timeval wait;
    wait.tv_sec = 0;
    wait.tv_usec = 500000;
    int res = select(maxfd,fds,NULL,NULL,&wait);
    if(res > 0){
        char buffer[1024];
    }
}

void * service_thread(void *){
    int res = 0;
    ss = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in serviceAddrs;
    bzero(&serviceAddrs,sizeof(sockaddr_in));
    serviceAddrs.sin_family = AF_INET;
    serviceAddrs.sin_addr.s_addr = INADDR_ANY;
    serviceAddrs.sin_port = htons(PORT);

    res = bind(ss,(sockaddr *)&serviceAddrs,sizeof(sockaddr_in));
    if(res != 0){
        printf("bind failed : %s \n",strerror(errno));
    }

    res = listen(ss,10);
    if(res != 0){
        printf("listen failed: %s \n",strerror(errno));
    }
    fd_set client_set;
    FD_ZERO(&client_set);
    int maxfd = -1;
    while(is_alive){
        sockaddr_in clientaddrs;
        bzero(&clientaddrs,sizeof(sockaddr_in));
        socklen_t addresslen = sizeof(sockaddr_in);
        int ssc = accept(ss,(sockaddr *)&clientaddrs,&addresslen);
        if(ssc < 0){
            printf("accept failed : %s \n",strerror(errno));
        }else{
            printf("client connect [%s:%d]\n",inet_ntoa(clientaddrs.sin_addr),htons(clientaddrs.sin_port));
            FD_SET(ssc,&client_set);
            if(ssc > maxfd){
                maxfd = ssc;
            }
            select_read(&client_set,maxfd + 1);
        }
    }
    close(ss);
    return 0;
}

void send_test(){
    char buffer[1024] = "socket client buffer";
    for(int i=0;i<10;i++){
        ssize_t sendbytes = send(sc,buffer,1024,MSG_DONTWAIT);
        printf("[%d] client send bytes %ld \n",i,sendbytes);
        sleep(1);
    }
}

void * client_thread(void *){
    int res = 0;
    sc = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in serviceAddrs;
    bzero(&serviceAddrs,sizeof(sockaddr_in));
    serviceAddrs.sin_family = AF_INET;
    serviceAddrs.sin_addr.s_addr = htonl(inet_network("127.0.0.1"));
    serviceAddrs.sin_port = htons(PORT);

    res = connect(sc,(sockaddr *)&serviceAddrs,sizeof(sockaddr));
    if(res != 0){
        printf("connect failed : %s \n",strerror(errno));
    }else{
        send_test();
    }
    close(sc);
    return 0;
}

void send_recv_test(){
    signal(SIGINT,process_sigint);
    signal(SIGPIPE,process_sigint);

    pthread_t service,client;

    pthread_create(&service,NULL,service_thread,NULL);
    pthread_create(&client,NULL,client_thread,NULL);

    pthread_join(service,NULL);
    pthread_join(client,NULL);
}