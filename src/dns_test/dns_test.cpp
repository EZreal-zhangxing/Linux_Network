#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<stdio.h>

void print_host_ent(hostent * ent){
    printf("offical name %s \n",ent->h_name);
    printf("protocl type: %d \n",ent->h_addrtype);

    char ** aliases = ent->h_aliases;
    while(*aliases){
        printf("host alias : %s \n",*aliases);
        aliases++;
    }

    char ** ipaddrs = ent->h_addr_list;
    char addr[100];
    while(*ipaddrs){
        printf("host ip : %s \n",inet_ntop(ent->h_addrtype,*ipaddrs,addr,100));
        ipaddrs++;
    }

}

void dns_test_method(){
    char hostAddrs[] ="www.baidu.com";
    hostent *ent;
    ent = gethostbyname(hostAddrs);
    print_host_ent(ent);
}