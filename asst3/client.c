#include "banking.h"
#include <time.h>

clock_t start = clock();
bool close = false;
void func(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

// Function to write commands to server
void writr(void * args)
{
    // type cast args to int
    int sockfd = (int) args;
    // Initialize buffer
    char buff[MAX];
    int n;
    // Continues to output commands forever
    while(close == false) {
        bzero(buff, sizeof(buff));
        printf("Enter a command : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        // after a command is inputed, wait for 2 seconds
        sleep(2);
    }
    // pthread_exit
}

// Function to read commands to server
void readr(void * args)
{
    // type cast args to int
    int sockfd = (int) args;
    // Initialize buffer
    char buff[MAX];
    // Continues to read messages forever
    while(close == false){
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));

        printf("From Server : %s", buff);
        if ((strncmp(buff, "Server is terminating program, DISCONNECTING\n", 45)) == 0) {
            printf("Client Exiting\n");
            close = true;
            break;
        }

    }
    // pthread_exit
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
    int tries = 15;

    while(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0 && tries>0){
        printf("connection with the server failed...trying again\n");
        tries--;
    }
    if(tries>0){
        printf("connected to the server\n");
    }else{
        printf("connection with the server failed too many times... exiting\n");
        exit(0);
    }

    // makes two threads for send and recieve
    pthread_t send;
    pthread_t recieve;
    // idk if attr is necessary
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // idk if attr is necessary
    if( pthread_create( &send , &attr ,  (void*)&writr , (void*) sockfd) < 0)
    {
        perror("could not create send thread");
        return -1;
    }
    if( pthread_create( &recieve , &attr ,  (void*)&readr , (void*) sockfd) < 0)
    {
        perror("could not create recieve thread");
        return -1;
    }

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}
