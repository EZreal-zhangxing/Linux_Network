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

static char eth_name[] = "enp172s0";
void process_multi_sig(int sig){
    keep_alive_multi = 0;
}

void * multicast_server(void *){

    int ss = socket(AF_INET,SOCK_DGRAM,0);

    sockaddr_in server_addr;
    bzero(&server_addr,sizeof(sockaddr_in));
    server_addr.sin_addr.s_addr = inet_network("224.1.1.10");
    server_addr.sin_port = htons(8888);
    server_addr.sin_family = AF_INET;


    ifreq ethinfo;
    memcpy(ethinfo.ifr_ifrn.ifrn_name,eth_name,sizeof(eth_name));
    ioctl(ss,SIOCGIFADDR,&ethinfo);
    sockaddr_in * temp_addr = (sockaddr_in *)&ethinfo.ifr_ifru.ifru_addr;
    printf("eth %s addr : %s \n",eth_name,inet_ntoa(temp_addr->sin_addr));

    ip_mreq req;
    socklen_t sockl = sizeof(req);
    memset(&req,0,sizeof(ip_mreq));

    req.imr_multiaddr.s_addr = temp_addr->sin_addr.s_addr;

    /* 从指定网卡发送 */
    setsockopt(ss,IPPROTO_IP,IP_MULTICAST_IF,&req,sockl);

    printf("multicast addr : %s \n",inet_ntoa(req.imr_multiaddr));
    printf("multicast ip addr : %s \n",inet_ntoa(req.imr_interface));

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
    return 0;
}


void test_multicast(){
    signal(SIGINT,process_multi_sig);
    multicast_server(0);
}