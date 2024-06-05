#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<sys/ioctl.h>
#include<net/if.h> // 网卡信息
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/if_ether.h>
#include<netinet/ip.h>

#include<netpacket/packet.h> // sockaddr_ll 设备无关物理地址结构
#include<unistd.h>

#include<signal.h>
#include<sys/time.h>

#include<errno.h>

static int is_capture = 1;

void signal_process_capture(int sig){
    is_capture = 0;
}

void capture_dataLink_data(char ** argv){
    char * eth_name = argv[1];
    signal(SIGINT,signal_process_capture);
    
    ifreq req;
    memcpy(req.ifr_ifrn.ifrn_name,eth_name,strlen(eth_name));

    // int ss = socket(AF_INET,SOCK_PACKET,htons(0x0003));
    int ss = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    ioctl(ss,SIOCGIFFLAGS,&req);
    short flags = req.ifr_ifru.ifru_flags;
    printf("eth %s flags %x \n",eth_name,flags);
    req.ifr_ifru.ifru_flags |= IFF_PROMISC;
    ioctl(ss,SIOCSIFFLAGS,&req);
    
    ioctl(ss,SIOCGIFADDR,&req);
    sockaddr_in* adds = (sockaddr_in *)&req.ifr_ifru.ifru_addr;
    printf("eth %s flags %x and addr %s\n",eth_name,req.ifr_ifru.ifru_flags,inet_ntoa(adds->sin_addr));

    // int res = setsockopt(ss,SOL_SOCKET,SO_BINDTODEVICE,eth_name,strlen(eth_name));
    // if(res < 0){
    //     printf("set sock opt failed \n");
    // }

    // int res = bind(ss,(sockaddr *)adds,sizeof(sockaddr_in));
    // if(res < 0){
    //     printf("bind failed \n");
    // }

    char buf[ETH_FRAME_LEN];
    ethhdr * ethhd;
    while(is_capture){
        ssize_t readBytes = read(ss,buf,ETH_FRAME_LEN);
        if(readBytes >= 0){
            ethhd = (ethhdr *)buf;
            printf("read from eth bytes [%4ld]",readBytes);
            printf(" source mac addr : ");
            for(int i=0;i<6;i++){
                printf("%02x:",ethhd->h_source[i]);
            }
            printf(" target mac addr : ");
            for(int i=0;i<6;i++){
                printf("%02x:",ethhd->h_dest[i]);
            }
            printf(" proto : %04x ",ntohs(ethhd->h_proto));
            if(ntohs(ethhd->h_proto) == 0x0800){
                iphdr * iphd = (iphdr *)(buf + ETH_HLEN);
                // printf("%x %x \n",iphd->saddr,iphd->daddr);
                unsigned long saddr = (iphd->saddr);
                unsigned long daddr = (iphd->daddr);
                printf("ip src %s ip dst %s \n",inet_ntoa(*((in_addr *)&saddr)),inet_ntoa(*((in_addr *)&daddr)));
            }else{
                printf("\n");
            }
        }else{
            printf("get error %s \n",strerror(errno));
        }
    }
    req.ifr_ifru.ifru_flags = flags;
    ioctl(ss,SIOCGIFFLAGS,&req);
    close(ss);
}