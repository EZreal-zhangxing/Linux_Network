#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>

#include<string.h>

#include<unistd.h>

#include <signal.h>
#include <time.h>

#define BROAD_STR "hello"
#define ETH_NAME "enx68da73ac8179"

static int broad_is_alive = 1;

void process_borad_sig(int sig){
    broad_is_alive = 0;
}

void broad_send_test(){
    signal(SIGINT,process_borad_sig);


    int ss = socket(AF_INET,SOCK_DGRAM,0);
    int res = 0;
    ifreq ethinfo;
    strncpy(ethinfo.ifr_ifrn.ifrn_name,ETH_NAME,sizeof(ETH_NAME));
    res = ioctl(ss,SIOCGIFBRDADDR,&ethinfo);

    int count =0 ;
    if(res < 0){
        printf("get eth broad cast addr failed \n");
    }else{
        sockaddr_in* addr = (sockaddr_in *)&ethinfo.ifr_ifru.ifru_broadaddr;
        printf("Eth: %s braod addr %s \n",ETH_NAME,inet_ntoa(addr->sin_addr));

        sockaddr_in sendAddr;
        bzero(&sendAddr,sizeof(sendAddr));
        memcpy((void *)&sendAddr.sin_addr,(void *)&addr->sin_addr,sizeof(sendAddr.sin_addr));
        sendAddr.sin_port = htons(8888);
        sendAddr.sin_family = AF_INET;
        int opt = 1;
        setsockopt(ss,SOL_SOCKET,SO_BROADCAST,&opt,sizeof(opt));
        time_t now;
        while(broad_is_alive){
            time(&now);
            sendto(ss,&now,sizeof(now),0,(sockaddr *)&sendAddr,sizeof(sendAddr));
            printf("boradcast a UDP packet [%3d]:[%ld] \n",count++,now);
            sleep(1);
        }
    }
    close(ss);
}