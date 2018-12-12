//
//  banking_h
//  ass3
//
//  Created by Joshua Olazo on 12/8/18.
//

#ifndef banking_h
#define banking_h

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
// define port and max for now
#define MAX 300
#define SA struct sockaddr
int* PORT;

//basic boolean definition
typedef enum { false, true } bool;
// account definition
typedef struct _account{
	char * name;
	double balance;
	bool flag;
	struct _account * next;
}account;

// client definition
typedef struct _client{
	pthread_t tid;
	int sock;
	struct _client * next;
}client;

typedef struct _thread_args{
	int sockfd;
	pthread_t readtid;
	pthread_t writetid;
}thread_args;

#endif /* banking_h */
