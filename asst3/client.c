#include "banking.h"

bool terminate = false;
// Function to write commands to server
void writr(void * args)
{
	// type cast args to int
    int* write_args = (int*) args;
	int sockfd = *write_args;
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
    int* read_args = (int*) args;
	int sockfd = *read_args;
    // Initialize buffer
    char buff[MAX];
	int n;
    // Continues to read messages forever
    while(terminate == false){
        bzero(buff, sizeof(buff));
        int bytes = read(sockfd, buff, sizeof(buff));

        printf("From Server : %s", buff);
		// if server turns off buff recives 0 bytes
        if (bytes == 0) {
			printf("Server is terminating program, DISCONNECTING\n");
            printf("Client Exiting\n");
            terminate = true;
			close(sockfd);
			pthread_exit(NULL);
            break;
        }


    }
	//printf("write pthread exit\n");
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

    // diagonostic prints
    printf("port is: %d \n", *PORT);


    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }else{
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // Get IP
    // Followed Beej.us
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }
    printf("IP addresses for %s:\n\n", argv[1]);
    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
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
        printf("  %s: %s\n", ipver, ipstr);
    }
    freeaddrinfo(res); // free the linked list of ips

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipstr);
    servaddr.sin_port = htons(*PORT);

    // connect the client socket to server socket tries 15 times


    while(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0){
        printf("connection with the server failed...trying again in 3 seconds\n");
        sleep(3);
    }
	printf("connected to the server\n");

    // makes two threads for send and recieve
    pthread_t send;
    pthread_t recieve;
    // idk if attr is necessary
    pthread_attr_t attr;
    pthread_attr_init(&attr);
	int * thread_args;
	thread_args=&sockfd;
    // idk if attr is necessary
    if( pthread_create( &send , &attr ,  (void*)&writr , (void*) thread_args ) < 0)
    {
        perror("could not create send thread");
        return -1;
    }
    if( pthread_create( &recieve , &attr ,  (void*)&readr , (void*) thread_args ) < 0)
    {
        perror("could not create recieve thread");
        return -1;
    }
	//printf("send tid: %d\n",send );
	//printf("recieve tid: %d\n",recieve );
    // function for chat
    pthread_join(send,NULL);
	printf("hey");
	pthread_cancel(recieve);

    // terminate the socket
    close(sockfd);
	return 0;
}
