#include<semaphore.h>
#include<stdio.h>
#include<pthread.h>
#include<signal.h>

sem_t  items;

static int is_alive = 1;

void signal_peocess(int sig){
    is_alive = 0;
    _exit(0);
}

void * creater_m(void * ){
    int val = 0;
    while(is_alive){
        sem_post(&items);
        sem_post(&items);
        sem_getvalue(&items,&val);
        printf(" items add %d \n",val);
        usleep(2000);
    }
    return NULL;
}


void * consumer_m(void * ){
    int val = 0;
    while(is_alive){
        sem_wait(&items);
        sem_getvalue(&items,&val);
        printf("items remove %d \n",val);
        usleep(100);
    }
    return NULL;
}

void sem_test(){
    pthread_t creater,consumer;

    signal(SIGINT,signal_peocess);

    sem_init(&items,0,10);

    pthread_create(&creater,NULL,creater_m,NULL);

    pthread_create(&consumer,NULL,consumer_m,NULL);

    pthread_join(creater,NULL);
    pthread_join(consumer,NULL);

    sem_destroy(&items);
}