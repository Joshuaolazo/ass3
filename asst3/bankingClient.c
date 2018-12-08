include "banking.h"

// Copied
char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[]) {
    if( argc != 2){
        fprintf(stderr, "%s\n", "wrong number of input args");
    }
    int port = argv[1];

    return 0;
}
