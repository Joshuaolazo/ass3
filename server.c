#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 256
#define PORT 12421
#define SA struct sockaddr


//basic boolean definition
typedef enum { false, true } bool;

typedef struct _account{
	char * name;
	double balance;
	bool flag;
	struct _account * next;
}account;

account * global;
bool isServiceSession;

//want to remove the command and the whitespace
bool nameAlreadyExists(char * input){
	if(global=NULL)
		return false;
	account * pointer = malloc(sizeof(account));
	pointer = global;
	while(pointer!=NULL){
		if(strcmp(input,pointer->name)==0)
			return true;

		pointer = pointer->next;
		}
	return false;
}

void createAccount(char * input){
	account * newAccount = malloc(sizeof(account));
	account * pointer;
	newAccount->name = input;
	newAccount->balance = 0;
	newAccount->flag = false;
	if(global==NULL)
		global = newAccount;
	else{
	//Attach account at end of GLOBAL LL
		pointer = global;
		while(pointer->next!=NULL){
			pointer=pointer->next;
			printf("%s->%s\n",pointer->name,pointer->flag);
		}

		pointer->next =  newAccount;

		}
	return;
}

char* trimcommand(char * input, int a){

	int i=0;
	int k=1;
	//remove first a letters
	for(i=0; i<a;i++){
		for(k=1;k<strlen(input);k++){
			input[k-1]= input[k];
			//printf("%s",input);
			}
		input[strlen(input)-1] = '\0';
	}

	//printf("first letters removed: %s\n",input);
	//remove leading whitespace
	while(input[0]==' '){
		for(k=1;k<strlen(input);k++){
			input[k-1] = input[k];
			//printf("%s",input);
		}

		input[strlen(input)-1] = '\0';
	}
	//printf("leading whitespace removed: %s\n",input);
	//remove trailing whitespace
	while(input[strlen(input)-1]==' '||input[strlen(input)-1]=='\n'){
		input[strlen(input)-1]= '\0';
		//printf("removed 1",input);
	}

	printf("%s\n",input);
	return input;
}
int isNumeric(char* data){
	int i = 0;
	int isNum = 1; //set to 0 if not numeric
	int period = 0; //shouldnt be more than one period
	int length = (int)(strlen(data));
	for(i=0; i<length;i++){
		char c = data[i];
		if(c < '0' || c > '9'){
			if(c == '.')
				period++;
			else
				isNum = 0;
			if(period > 1)
				isNum = 0;
		}
	}
	return isNum;
}

void metadata()
{
    //sem_wait(&mutex);
    printf("Beginning Metadata Dump\n");
    account* ptr =(account*) malloc(sizeof(account));
    ptr = global;
    while(ptr->next != NULL){

        char* accountname= ptr->name;
        double accountbalance= ptr->balance;
        if(ptr->flag == true){
            printf("%s\t%s\tIN SERVICE\n", accountname,accountbalance);
        }else{
            printf("%s\t%s\n", accountname,accountbalance);
        }
    }
    //sem_post(&mutex);

}

// Function designed for chat between client and server.
void func(int sockfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));

        //My shitty code

        //example account created set to flagged account if not last node
		  account * example;
		  example = global;
		  bool flagFound = false;

		  while(example->next!=NULL){
		  if(example->flag){
		  		flagFound=true;
		  		break;
		  	}

		  	example= example->next;
		  }

        char * inputcopy = malloc(strlen(buff)*sizeof(char)*2);
        strcpy(inputcopy,buff);


     //CREATE ACCOUNT
        if(strncmp("create",buff,6)==0){
        	strcpy(inputcopy,trimcommand(inputcopy,6));
        	if(isServiceSession)
        		printf("Cannot create new account during a service session.\n");
        	else{
        		if(nameAlreadyExists(inputcopy))
        			printf("The account name you seek to create already exists.\n");
        		else{
        			if(strlen(inputcopy)>255)
        				printf("The account name cannot exceed 255 characters.\n");
        			else {
        				//CREATE NEW ACCOUNT
        				createAccount(inputcopy);
        				printf("Account: --%s-- has been successfully created.\n",inputcopy);
        			}
        		}
        	}
			metadata();
     	}

     //SERVICE
        else if(strncmp("serve",buff,5)==0){
        	strcpy(inputcopy,trimcommand(inputcopy,5));

        	//FIND ACCOUNT TO SERVICE BY NAME
        	bool nameMatch = false;
        	example = global;
		  	while(example!=NULL){
		  		if(strcmp(inputcopy,example->name)==0){
		  			nameMatch = true;
		  			break;
		  		}
		  		example = example->next;
		  	}
        	//if Name NOT FOUND
        	if(!nameMatch)
        		printf("Account name to be serviced cannot be found.\n");

        		else{

        			if(example->flag||isServiceSession)		//Might need to differentiate between single service on client, and no concurrent serve on same account across multiple clients.
        				printf("Error: Cannot service. An account is already in a service session.\n");

       		 	else{
        				example->flag = true;
        				isServiceSession = true;
        				printf("%d\n",isServiceSession);
       				printf("Account with name: --%s-- :is now IN service\n",inputcopy);
       	}
       }
     }

     //DEPOSIT
        else if(strncmp("deposit",buff,7)==0){
        		if(example->flag){
        			strcpy(inputcopy,trimcommand(inputcopy,7));
        			if(isNumeric(inputcopy)){
       		 		example->balance = example->balance + atof(inputcopy);
        				printf("Balance of account --%s-- after deposit: %f\n",example->name,example->balance);
 				}
 					else
 						printf("Input is not Numeric.\n");
 			}
 				else
 					printf("Cannot Deposit, account is not in service.\n");
     }

     //WITHDRAW
        else if(strncmp("withdraw",buff,8)==0){
        		if(example->flag){
        			strcpy(inputcopy,trimcommand(inputcopy,8));
        			if(isNumeric(inputcopy)){

        				if(example->balance<atof(inputcopy))
        					printf("Withdraw request is invalid. ");
        				else
        					example->balance = example->balance - atof(inputcopy);

        				printf("Balance of account --%s-- after withdraw: %f\n",example->name,example->balance);
        		}
      		  else
 						printf("Input is not Numeric.\n");
 			}
 				else
 					printf("Cannot Withdraw, account is not in service.\n");
     }

     //QUERY
        else if(strncmp("query",buff,5)==0){
        		if(example->flag){
       			printf("Current account balance is: %f\n",example->balance);
       		}
       		else
       			printf("Cannot perform Query, account is not in service.\n");
     }

     //END SERVICE
        else if(strncmp("end",buff,3)==0){

         	strcpy(inputcopy,trimcommand(inputcopy,3));

      		if(!example->flag)
        			printf("Error: The account service session has already been ended.\n");
       	 	else{
        			example->flag = false;
        			isServiceSession = false;
       			printf("Account with name: --%s-- :is now OUT OF service\n",inputcopy);
       	}
     }

     //QUIT
        else if(strncmp("quit",buff,4)==0){
      	  printf("quit\n");
     }
        else
       	 printf("error: %s does not contain a valid command\n",buff);








        //end shit code

        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
		/*
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;

        // and send that buffer to client
        write(sockfd, buff, sizeof(buff));

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
		*/
    }
}

// Driver function
int main()
{

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
    servaddr.sin_port = htons(PORT);

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

      isServiceSession= false;
		global = malloc(sizeof(account));

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);
}
