#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <net/route.h>

#include<pthread.h>
#include<signal.h>
#include<errno.h>

static int is_alive = 1;

void signal_p(int sig){
    is_alive = 0;
    exit(0);
}

void get_eth_info(int argc,char * argv[]){
    int s = socket(AF_INET,SOCK_DGRAM,0);
    if(s < 0){
        printf("err make socket \n");
        return;
    }
    struct ifreq req;
    req.ifr_ifindex = atoi(argv[1]);
    int err = 0;

    err = ioctl(s,SIOCGIFNAME,&req);
    if(!err){
        printf("eth name is %s \n",req.ifr_name);
    }
    err = ioctl(s,SIOCGIFADDR,&req);
    if(!err){
        struct sockaddr_in * sin = (struct sockaddr_in * )&req.ifr_addr;
        printf("eth addr is %s \n",inet_ntoa(sin->sin_addr));
    }

    err = ioctl(s,SIOCGIFNETMASK,&req);
    if(!err){
        struct sockaddr_in * sin_mask = (struct sockaddr_in * )&req.ifr_netmask;
        printf("eth net mask is %s \n", inet_ntoa(sin_mask->sin_addr));
    }

    err = ioctl(s,SIOCGIFHWADDR,&req);
    if(!err){
        printf("eth hardwave addr is %02x:%02x:%02x:%02x:%02x:%02x \n"
            ,req.ifr_hwaddr.sa_data[0],req.ifr_hwaddr.sa_data[1],req.ifr_hwaddr.sa_data[2]
            ,req.ifr_hwaddr.sa_data[3],req.ifr_hwaddr.sa_data[4],req.ifr_hwaddr.sa_data[5]);
    }
    
    close(s);
}

void* service_test(void *){
    int s = socket(AF_INET,SOCK_STREAM,0);
    int res = 0;
    sockaddr_in saddr;

    bzero(&saddr,sizeof(sockaddr_in));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(10000);

    res = bind(s,(sockaddr *)&saddr,sizeof(sockaddr_in));

    if(res < 0){
        printf("bind failed %d %s\n",res,strerror(errno));
    }else{
        printf("bind success %d \n",res);
    }

    res = listen(s,10);
    if(res < 0){
        printf("listen failed \n");
    }
    while(1){
        sockaddr_in clientaddr;
        socklen_t addl = sizeof(sockaddr_in);
        int clientsfd = accept(s,(sockaddr *)&clientaddr,&addl);
        printf("client connected [%s] socket id [%d]\n",inet_ntoa(clientaddr.sin_addr),clientsfd);
        close(clientsfd);
    }

    close(s);
    return NULL;
}

void * connect_test(void *){
    sleep(1);
    int s = socket(AF_INET,SOCK_STREAM,0);
    int res = 0;

    sockaddr_in serviceaddr;

    bzero(&serviceaddr,sizeof(sockaddr_in));
    serviceaddr.sin_family = AF_INET;
    serviceaddr.sin_port = htons(10000);
    serviceaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    // serviceaddr.sin_addr.s_addr = htonl(inet_network("127.0.0.1"));
    res = connect(s,(sockaddr *)&serviceaddr,sizeof(serviceaddr));
    if(res == 0){
        printf("connect success \n");
    }else{
        printf("connect failed %d %s\n",res,strerror(errno));
    }
    close(s);
    return NULL;
}

void test_socket(){
    pthread_t service,client;
    long a=10;
    short b = 10;
    printf("sizeof long %d \n",sizeof(a));
    printf("sizeof short %d \n",sizeof(b));
    signal(SIGINT,signal_p);

    pthread_create(&service,NULL,service_test,NULL);

    pthread_create(&client,NULL,connect_test,NULL);

    pthread_join(service,NULL);
    pthread_join(client,NULL);

}