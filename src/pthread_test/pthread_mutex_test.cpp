#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<signal.h>

pthread_mutex_t count;
static int end = 0;
static int num = 0;

void signal_process(int sig){
    printf("this is need exit \n");
    end = 1;
}

void* thread_1_method(void *){
    while (!end)
    {
        pthread_mutex_lock(&count);
        num ++;
        printf("this is thread 1 method %d \n",num);
        pthread_mutex_unlock(&count);
        usleep(100);
    }
    return NULL;
}

void* thread_2_method(void *){
    while (!end){
        pthread_mutex_lock(&count);
        num--;
        printf("this is thread 2 method %d\n",num);
        pthread_mutex_unlock(&count);
        usleep(100);
    }
    return NULL;
}

void thread_test(){
    signal(SIGINT,signal_process);

    pthread_mutex_init(&count,NULL);
    pthread_t thread1,thread2;
    pthread_create(&thread1,NULL,thread_1_method,NULL);
    pthread_create(&thread2,NULL,thread_2_method,NULL);

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_mutex_destroy(&count);
    printf("main method is finished \n");

}