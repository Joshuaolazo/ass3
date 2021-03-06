#include "banking.h"



bool terminate = false;
// Function to write commands to server
void writr(void * args)
{
	// type cast args to int
	thread_args * in_args = (thread_args *) args;
	int sockfd = in_args->sockfd;
    // Initialize buffer
    char buff[MAX];
    int n;
    // Continues to output commands forever
    while(terminate == false) {
        bzero(buff, sizeof(buff));
        printf("Enter a command : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        // after a command is inputed, wait for 2 seconds
		// maybe not sleepa
        sleep(2);
    }
	printf("read pthread exit\n");
    pthread_exit(NULL);
}

// Function to read commands to server
void readr(void * args)
{
    // type cast args to int
    thread_args * in_args = (thread_args *) args;
	int sockfd = in_args->sockfd;
    // Initialize buffer
    char buff[MAX];
	int n;
    // Continues to read messages forever
    while(terminate == false){
		bzero(buff, sizeof(buff));
		int bytes = read(sockfd, buff, sizeof(buff));

		printf("From Server : %s", buff);
		// if server turns off buff recives 0 bytes or sent message to exit
		if (strcmp( buff, "Client has been disconnected.\n")==0 || strcmp( buff, "Server is terminating program, DISCONNECTING\n")==0){
			printf("Client Exiting\n");
			terminate = true;
			close(sockfd);
			pthread_cancel(in_args->writetid);
			pthread_exit(NULL);
			break;
		}
		if (bytes == 0 ) {
			printf("Server disconnected, bytes recieved 0\n");
			printf("Client Exiting\n");
			terminate = true;
			close(sockfd);
			//printf("canceling: %i\n",in_args->writetid );
			pthread_cancel(in_args->writetid);
			pthread_exit(NULL);
			break;
		}
    }
    pthread_exit(NULL);
}
// main function takes in args: ./client servername port
int main(int argc, char const *argv[])
{
    // Strictly needs 3 arguments
    if( argc != 3){
        fprintf(stderr, "%s\n", "wrong number of input args");
    }
    // Reads the last arg, the port, converts it to int, and updates global port
    int port_int = atoi(argv[2]);
    PORT = &port_int;

    int sockfd;
    struct sockaddr_in serveraddress;

    // Creating socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket Creating Failure, exiting program\n");
        exit(0);
    }else{
        printf("Initial Socket has been created\n");
    }
    bzero(&serveraddress, sizeof(serveraddress));

    // Get IP boilerplate
    // Followed Beej.us
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(0);
    }
	// tries every ip for hostname
    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    }
    freeaddrinfo(res); // free the linked list of ips

    serveraddress.sin_family = AF_INET;
    serveraddress.sin_addr.s_addr = inet_addr(ipstr);
    serveraddress.sin_port = htons(*PORT);

    // connect the client socket to server socket tries unlimited times

    while(connect(sockfd, (SA*)&serveraddress, sizeof(serveraddress)) != 0){
        printf("Server Client Connection Failure trying again in 3 seconds\n");
        sleep(3);
    }
	printf("Connected to the server!\n");

    // makes two threads for send and recieve
    pthread_t send;
    pthread_t recieve;
    // idk if attr is necessary
    pthread_attr_t attr;
    pthread_attr_init(&attr);
	thread_args * t_arg = malloc(sizeof(thread_args));
	t_arg->readtid = recieve;
	t_arg->writetid = send;
	t_arg->sockfd = sockfd;
	if( pthread_create( &recieve , &attr ,  (void*)&readr , (void*) t_arg ) < 0)
	{
		perror("could not create recieve thread");
		exit(0);
	}
	t_arg->readtid = recieve;
    if( pthread_create( &send , &attr ,  (void*)&writr , (void*) t_arg ) < 0)
    {
        perror("could not create send thread");
        exit(0);
    }
	t_arg->readtid = recieve;
	t_arg->writetid = send;

	//printf("send tid: %d\n",send );
	//printf("recieve tid: %d\n",recieve );
    // function for chat
    pthread_join(send,NULL);
	pthread_cancel(recieve);

    // terminate the socket
    close(sockfd);
	exit(0);
}
