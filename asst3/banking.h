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
// define port and max for now
#define MAX 256
#define SA struct sockaddr


//basic boolean definition
typedef enum { false, true } bool;
// account definition
typedef struct _account{
	char name[256];
	double balance;
	bool flag;
}account;


#endif /* banking_h */
