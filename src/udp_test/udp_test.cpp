#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>
#include<sys/time.h>

#include<unistd.h>
#include<pthread.h>
#include<signal.h>
#include<map>

#include<string>

#define PORT 8888

static int is_alive = 1;

std::map<int,struct sockaddr_in> client_map;

void process_udp_signal(int sig){
    is_alive = 0;
}

void* service_thread_udp(void *){
    int ss = socket(AF_INET,SOCK_DGRAM,0);

    sockaddr_in serviceAddr;
    bzero(&serviceAddr,sizeof(sockaddr_in));

    serviceAddr.sin_family = AF_INET;
    serviceAddr.sin_port = htons(PORT);
    serviceAddr.sin_addr.s_addr = INADDR_ANY;

    int res = 0;

    res = bind(ss,(sockaddr *)&serviceAddr,sizeof(sockaddr_in));
    if(res < 0){
        printf("bind addr failed : %s\n",strerror(errno));
    }
    char buf[1024];
    char ack[100];
    while(is_alive){
        
        sockaddr_in clientAddr;
        socklen_t addrSize = sizeof(sockaddr_in);
        bzero(&clientAddr,addrSize);

        ssize_t recvbytes = recvfrom(ss,buf,1024,MSG_DONTWAIT,(sockaddr *)&clientAddr,&addrSize);
        if(recvbytes > 0){
            std::string temp = buf;
            const char * subtemp = temp.substr(temp.find_first_of(','),temp.length()).c_str();
            sprintf(ack,"%s %s",inet_ntoa(clientAddr.sin_addr),subtemp);
            printf("[service] : recv from %s:%d bytes [%ld]:[%s] and ack for %s \n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port),recvbytes,buf,ack);
            sendto(ss,ack,100,MSG_DONTWAIT,(sockaddr *)&clientAddr,addrSize);
        }
        usleep(200000);
    }
    close(ss);
    return 0;
}

void* client_thread_udp(void * argv){
    int sc = socket(AF_INET,SOCK_DGRAM,0);
    int threadId = *(int *)argv;
    sockaddr_in serviceAddr;
    bzero(&serviceAddr,sizeof(sockaddr_in));

    serviceAddr.sin_family = AF_INET;
    serviceAddr.sin_port = htons(PORT);
    serviceAddr.sin_addr.s_addr = htonl(inet_network("127.0.0.1"));

    int res = 0;

    char buf[1024] = "";
    char ack[100] = "";
    sockaddr_in recvfromAddr;
    socklen_t addrSize = sizeof(sockaddr_in);
    bzero(&recvfromAddr,addrSize);
    int packet_count = 0;
    while(is_alive){
        sprintf(buf,"%s %d,%d","Hello from ",threadId,packet_count++);
        ssize_t sendbytes = sendto(sc,buf,1024,MSG_DONTWAIT,(sockaddr *)&serviceAddr,sizeof(sockaddr_in));
        if(sendbytes > 0){
            
            ssize_t recvbytes = recvfrom(sc,ack,100,MSG_DONTWAIT,(sockaddr *)&recvfromAddr,&addrSize);
            if(recvbytes > 0){
                printf("[client]: send to %s:%d bytes [%ld]:[%s] recv [%ld] ack : %s \n"
                    ,inet_ntoa(serviceAddr.sin_addr),ntohs(serviceAddr.sin_port),sendbytes,buf,recvbytes,ack);
            }
        }
        sleep(1);    
    }
    close(sc);
    return 0;
}

int main_test_udp(int argc,char * argv[]){
    signal(SIGINT,process_udp_signal);
    int client_num = 1;
    pthread_t service,client[client_num];
    pthread_create(&service,NULL,service_thread_udp,NULL);
    for(int i=0;i<client_num;i++){
        pthread_create(client+i,NULL,client_thread_udp,&i);
    }
    
    
    for(int i=0;i<client_num;i++){
        pthread_join(client[i],NULL);
    }
    pthread_join(service,NULL);
    return 0;
}