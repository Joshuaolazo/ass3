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
#include <unistd.h>
#include <signal.h>
// define port and max for now
#define MAX 256
#define SA struct sockaddr
int* PORT;

//basic boolean definition
typedef enum { false, true } bool;
// account definition
typedef struct _account{
	char name[256];
	double balance;
	bool flag;
}account;


#endif /* banking_h */
