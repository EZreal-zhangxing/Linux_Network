#include<sys/socket.h>
#include<sys/ioctl.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include <errno.h>
#include<string.h>
#include<pthread.h>

#include<net/if.h>
#include<signal.h>
static int keep_alive_multi = 1;

// static char eth_name[] = "wlp0s20f3";
void process_multi_sig(int sig){
    keep_alive_multi = 0;
}

void * multicast_server(void * argv){

    int ss = socket(AF_INET,SOCK_DGRAM,0);
    char ** t_argv = (char **)argv;
    char * eth_name = (char *)t_argv[1];
    sockaddr_in server_addr;
    bzero(&server_addr,sizeof(sockaddr_in));
    server_addr.sin_addr.s_addr = htonl(inet_network("224.1.1.10"));
    server_addr.sin_port = htons(8888);
    server_addr.sin_family = AF_INET;


    ifreq ethinfo;
    memcpy(ethinfo.ifr_ifrn.ifrn_name,eth_name,strlen(eth_name));
    ioctl(ss,SIOCGIFADDR,&ethinfo);
    sockaddr_in * temp_addr = (sockaddr_in *)&ethinfo.ifr_ifru.ifru_addr;
    printf("eth %s addr : %s \n",ethinfo.ifr_ifrn.ifrn_name,inet_ntoa(temp_addr->sin_addr));

    socklen_t sockl = sizeof(sockaddr_in);

    /* 设置发送地址，从指定网卡发送 */
    setsockopt(ss,IPPROTO_IP,IP_MULTICAST_IF,temp_addr,sockl);

    // printf("multicast addr : %s \n",inet_ntoa(req.imr_multiaddr));
    // printf("multicast ip addr : %s \n",inet_ntoa(req.imr_interface));

    char multi_str[] = "MULTICAST HELLO";
    while(keep_alive_multi){
        
        ssize_t send_size = sendto(ss,multi_str,sizeof(multi_str),MSG_DONTWAIT,(sockaddr *)&server_addr,sizeof(sockaddr_in));
        if(send_size >= 0){
            printf("send Multicast Packet bytes[%3ld] \n",send_size);
        }else{
            printf("send failed:%s \n",strerror(errno));
        }
        sleep(1);
    }
    close(ss);
    return 0;
}


void * multicast_client(void * argv){
    int sc = socket(AF_INET,SOCK_DGRAM,0);
    
    char ** t_argv = (char **)argv;
    char * eth_name = (char *)t_argv[1];
    int res = 0;
    ifreq ethinfo;

    memcpy(ethinfo.ifr_ifrn.ifrn_name,eth_name,strlen(eth_name));
    res = ioctl(sc,SIOCGIFADDR,&ethinfo);
    if(res < 0){
        printf("get eth info error \n");
        return 0;
    }
    sockaddr_in * addr = (sockaddr_in *)&ethinfo.ifr_ifru.ifru_addr;
    printf("multicast client eth addr %s \n",inet_ntoa(addr->sin_addr));

    sockaddr_in localAddr;
    bzero(&localAddr,sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(8888);

    res = bind(sc,(sockaddr *)&localAddr,sizeof(localAddr));
    if(res < 0){
        printf("bind failed : %s\n",strerror(errno));
    }
    // int ttl = 32;
    // setsockopt(sc,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,sizeof(int));
    int loop = 1;
    setsockopt(sc,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(int));

    ip_mreq req;

    req.imr_multiaddr.s_addr = htonl(inet_network("224.1.1.10"));
    req.imr_interface.s_addr = addr->sin_addr.s_addr;

    res = setsockopt(sc,IPPROTO_IP,IP_ADD_MEMBERSHIP,&req,sizeof(req));
    if(res < 0){
        printf("add membership failed %s\n",strerror(errno));
    }
    
    char buf[100];
    sockaddr_in recv_addr;
    socklen_t addlen = sizeof(sockaddr_in);
    bzero(&recv_addr,sizeof(sockaddr_in));

    timeval waitTime;
    waitTime.tv_sec = 1;
    waitTime.tv_usec = 0;
    while(keep_alive_multi){
        int bytes = recvfrom(sc,buf,sizeof(buf),MSG_DONTWAIT,(sockaddr *)&recv_addr,&addlen);
        if(bytes >= 0){
            printf("recv from  %s:%d bytes [%d]:%s \n",inet_ntoa(recv_addr.sin_addr),ntohs(recv_addr.sin_port),bytes,buf);
        }else{
            printf("rece occur error : %s \n",strerror(errno));
        }
        
        sleep(1);
    }
    setsockopt(sc,IPPROTO_IP,IP_DROP_MEMBERSHIP,&req,sizeof(req));
    close(sc);
    return 0;
}


void test_multicast(int argc,char * argv[]){
    signal(SIGINT,process_multi_sig);
    if(argc == 1){
        printf("please execute ./main ehtname\n");
        exit(-1);
    }
    pthread_t multi_server,multi_client;
    pthread_create(&multi_server,NULL,multicast_server,argv);
    pthread_create(&multi_client,NULL,multicast_client,argv);

    pthread_join(multi_server,NULL);
    pthread_join(multi_client,NULL);

}