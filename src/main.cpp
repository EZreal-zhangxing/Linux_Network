#include "event_test/select_event_test.h"
#include "socketopt/socketopt_test.h"
#include "file_test/file_test.h"
#include "pipe_connect/pipe_test.h"
#include "pthread_test/pthread_mutex_test.h"
#include "TCP_test/tcp_test.h"

int main(int argc,char * argv[]){
    // select_event_test(argc,argv);
    // get_eth_info(argc,argv);
    // test_file("test.txt");
    // pipe_test(argc,argv);
    // pipe_write_read();
    // thread_test();
    // sem_test();
    // test_socket();
    test_tcp_connect();
    return 0;
}
