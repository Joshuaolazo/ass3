#include "banking.h"

// global booleans to close and print
bool close = false;
bool print = false;
// Function designed for chat between client and server.
void func(int sockfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (close == false) {

        // server only reads
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));

        //My shitty code
        account * example = malloc(sizeof(account));
        char * inputcopy = malloc(strlen(buff)*sizeof(char)*2);
        strcpy(inputcopy,buff);
        example->balance = 100;

        if(strncmp("create",buff,6)==0){
        printf("create\n");
     }
        else if(strncmp("serve",buff,5)==0){
        printf("serve\n");
     }
        else if(strncmp("deposit",buff,7)==0){
        strsep(&inputcopy,"deposit");
        printf("deposit%s\n",inputcopy);
     }
        else if(strncmp("withdraw",buff,8)==0){
        printf("withdraw\n");
     }
        else if(strncmp("query",buff,5)==0){
        printf("query\n");
     }
        else if(strncmp("end",buff,3)==0){
        printf("end\n");
     }
        else if(strncmp("quit",buff,4)==0){
        printf("quit\n");
     }
        else
        printf("error: %s does not contain a valid command\n",buff);


        //end shit code

        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
    }
    bzero(buff, MAX);
    buff = "Server is terminating program, DISCONNECTING\n"
    write(sockfd, buff, sizeof(buff));
    //pthread_exit or return
}


void sig_handler(int signum)
{
    if(signum== SIGINT){
        close = true;
    }else if (signum == SIGALARM) {
        print = true;
    }else{
        fprintf(stderr, "Recieved a bad signum, got: %d closing anyways\n",signum);
        close = true;
    }
}

void program_ender(void* args)
{
    printf("Control C pressed, exiting program\n");
    printf("Stoping Timer\n");
    printf("Locking all accounts\n");
    printf("Disconnecting all clients\n");
    printf("Sending all clients shutdown messages\n");
    printf("Deallocating all memory lol\n");
    printf("Closing all sockets\n");
    printf("Joining all threadss\n");
}

void metadata(void * args)
{

    printf("Beginning Metadata Dump\n");
    Node* ptr =(Node*) malloc(sizeof(Node));
    prt = LISTOFCLIENTS;
    while(prt->next != NULL){

        char* accountname= ptr->account->name;
        double accountbalance= prt->account->balance;
        if(ptr->account->flag == true){
            printf("%s\t%s\tIN SERVICE\n", accountname,accountbalance)
        }else{
            printf("%s\t%s\n", accountname,accountbalance)
        }
    }
    

}

// Driver function
int main(int argc, char const *argv[])
{
	if( argc != 2){
		fprintf(stderr, "%s\n", "wrong number of input args");
	}

    int p = atoi(argv[1]);
	PORT = &p;
	printf("port is: %d ", *PORT);

    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(*PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);
}
